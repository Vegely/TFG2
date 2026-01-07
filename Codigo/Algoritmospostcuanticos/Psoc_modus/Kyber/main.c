/******************************************************************************
* File Name:   main.c
* Description: Kyber Agent + Verification (With Safety Delay)
******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cy_crypto_core.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kyber_wrapper.h"

#define MAX_BUF_SIZE 4096

bool UART_ReceivePacket(unsigned char* buf, uint32_t* outLen) {
    int low = getchar();
    int high = getchar();
    if(low == EOF || high == EOF) return false;

    uint16_t len = (uint16_t)(low | (high << 8));
    if (len > MAX_BUF_SIZE) return false;

    for (uint16_t i = 0; i < len; i++) {
        int c = getchar();
        if (c == EOF) return false;
        buf[i] = (unsigned char)c;
    }
    *outLen = len;
    return true;
}


void UART_SendPacket(const unsigned char* data, uint32_t len) {
    putchar(len & 0xFF);
    putchar((len >> 8) & 0xFF);
    for (uint32_t i = 0; i < len; i++) {
        putchar(data[i]);
    }
}

int main(void)
{
    /* 1. Init Hardware */
    if (cybsp_init() != CY_RSLT_SUCCESS) CY_ASSERT(0);
    __enable_irq();
    if (cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200) != CY_RSLT_SUCCESS) CY_ASSERT(0);

    /* 2. Init Crypto */
    Cy_Crypto_Core_Enable(CRYPTO);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    /* 3. Init Kyber */
    if (kyber_init(KYBER_SECURITY_1024) != 0) { /* Error */ }

    // Allocate Memory
    size_t sz_pk = kyber_get_public_key_size();
    size_t sz_sk = kyber_get_secret_key_size();
    size_t sz_ct = kyber_get_ciphertext_size();
    size_t sz_ss = kyber_get_shared_secret_size();

    unsigned char *pk = calloc(sz_pk, 1);
    unsigned char *sk = calloc(sz_sk, 1);
    unsigned char *ct = calloc(sz_ct, 1);
    unsigned char *ss = calloc(sz_ss, 1);
    unsigned char *rx_buf = calloc(MAX_BUF_SIZE, 1);

    if (!pk || !sk || !rx_buf) while(1); 

    printf("READY"); 

    for(;;)
    {
        int cmd = getchar(); 

        // ECHO MODE
        if (cmd == 'E') {
            uint32_t len = 0;
            if (UART_ReceivePacket(rx_buf, &len)) {
                UART_SendPacket(rx_buf, len);
            }
        }

        // ALICE MODE
        else if (cmd == 'A') 
        {
            kyber_keypair(pk, sk);
            UART_SendPacket(pk, (uint32_t)sz_pk);

            uint32_t len = 0;
            if (UART_ReceivePacket(rx_buf, &len)) {
                if (len == sz_ct) {
                    memcpy(ct, rx_buf, sz_ct);
                    kyber_decapsulate(ss, ct, sk);
                    

                    // Wait 500ms to let Windows switch to "Receive Mode"
                    cyhal_system_delay_ms(500); 
                    
                    // Send Secret for Verification
                    UART_SendPacket(ss, (uint32_t)sz_ss);
					
                }
            }
        }

        // BOB MODE
        else if (cmd == 'B') 
        {
            uint32_t len = 0;
            if (UART_ReceivePacket(rx_buf, &len)) {
                if (len == sz_pk) {
                    memcpy(pk, rx_buf, sz_pk);
                    kyber_encapsulate(ct, ss, pk);
                    UART_SendPacket(ct, (uint32_t)sz_ct);
                    

                    // Wait 500ms to let Windows switch to "Receive Mode"
                    cyhal_system_delay_ms(500);

                    // Send Secret for Verification
                    UART_SendPacket(ss, (uint32_t)sz_ss);

                }
            }
        }
    }
}