#include "types.h"
#include <string.h>

typedef unsigned char uint8_t;

// Helper function to get bit at position i in byte array
static int get_bit(const uint8_t *poly, int bit_pos) {
    int byte_idx = bit_pos / 8;
    int bit_idx = bit_pos % 8;
    return (poly[byte_idx] >> bit_idx) & 1;
}

// Helper function to set bit at position i in byte array
static void set_bit(uint8_t *poly, int bit_pos) {
    int byte_idx = bit_pos / 8;
    int bit_idx = bit_pos % 8;
    poly[byte_idx] |= (1 << bit_idx);
}

// Helper function to clear bit at position i in byte array
static void clear_bit(uint8_t *poly, int bit_pos) {
    int byte_idx = bit_pos / 8;
    int bit_idx = bit_pos % 8;
    poly[byte_idx] &= ~(1 << bit_idx);
}

// Helper function to find degree of polynomial (highest set bit)
static int poly_degree(const uint8_t *poly, int max_bytes) {
    for (int i = max_bytes * 8 - 1; i >= 0; i--) {
        if (get_bit(poly, i)) {
            return i;
        }
    }
    return -1; // Zero polynomial
}

// Helper function to copy polynomial
static void poly_copy(uint8_t *dest, const uint8_t *src, int size) {
    memcpy(dest, src, size);
}

// Helper function to zero polynomial
static void poly_zero(uint8_t *poly, int size) {
    memset(poly, 0, size);
}

// GF(2) polynomial addition (XOR)
void ntl_add(uint8_t res_bin[R_SIZE],
             const uint8_t a_bin[R_SIZE],
             const uint8_t b_bin[R_SIZE]) {
    for (int i = 0; i < R_SIZE; i++) {
        res_bin[i] = a_bin[i] ^ b_bin[i];
    }
}

// GF(2) polynomial modular multiplication
void ntl_mod_mul(uint8_t res_bin[R_SIZE],
                 const uint8_t a_bin[R_SIZE],
                 const uint8_t b_bin[R_SIZE]) {
    uint8_t temp[2 * R_SIZE];
    uint8_t modulus[R_SIZE + 1];
    
    // Clear result arrays
    poly_zero(temp, 2 * R_SIZE);
    poly_zero(modulus, R_SIZE + 1);
    
    // Multiply a * b (schoolbook multiplication in GF(2))
    for (int i = 0; i < R_BITS; i++) {
        if (get_bit(a_bin, i)) {
            for (int j = 0; j < R_BITS; j++) {
                if (get_bit(b_bin, j)) {
                    int pos = i + j;
                    if (get_bit(temp, pos)) {
                        clear_bit(temp, pos);
                    } else {
                        set_bit(temp, pos);
                    }
                }
            }
        }
    }
    
    // Create modulus: x^R_BITS + 1
    set_bit(modulus, 0);      // coefficient of x^0
    set_bit(modulus, R_BITS); // coefficient of x^R_BITS
    
    // Polynomial division/reduction modulo (x^R_BITS + 1)
    int temp_degree = poly_degree(temp, 2 * R_SIZE);
    int mod_degree = R_BITS;
    
    while (temp_degree >= mod_degree) {
        int shift = temp_degree - mod_degree;
        
        // Subtract (XOR) modulus shifted by 'shift' positions
        for (int i = 0; i <= mod_degree; i++) {
            if (get_bit(modulus, i)) {
                int pos = i + shift;
                if (get_bit(temp, pos)) {
                    clear_bit(temp, pos);
                } else {
                    set_bit(temp, pos);
                }
            }
        }
        
        temp_degree = poly_degree(temp, 2 * R_SIZE);
    }
    
    // Copy result (lower R_SIZE bytes)
    memcpy(res_bin, temp, R_SIZE);
}

// Extended Euclidean Algorithm for GF(2) polynomials
void ntl_mod_inv(uint8_t res_bin[R_SIZE],
                 const uint8_t a_bin[R_SIZE]) {
    uint8_t u[R_SIZE + 1], v[R_SIZE + 1];
    uint8_t g1[R_SIZE + 1], g2[R_SIZE + 1];
    uint8_t temp[R_SIZE + 1];
    
    // Initialize
    poly_zero(u, R_SIZE + 1);
    poly_zero(v, R_SIZE + 1);
    poly_zero(g1, R_SIZE + 1);
    poly_zero(g2, R_SIZE + 1);
    
    // u = a, v = modulus (x^R_BITS + 1)
    memcpy(u, a_bin, R_SIZE);
    set_bit(v, 0);
    set_bit(v, R_BITS);
    
    // g1 = 1, g2 = 0
    set_bit(g1, 0);
    
    while (poly_degree(u, R_SIZE + 1) != 0) {
        int u_deg = poly_degree(u, R_SIZE + 1);
        int v_deg = poly_degree(v, R_SIZE + 1);
        
        if (u_deg < v_deg) {
            // Swap u,v and g1,g2
            memcpy(temp, u, R_SIZE + 1);
            memcpy(u, v, R_SIZE + 1);
            memcpy(v, temp, R_SIZE + 1);
            
            memcpy(temp, g1, R_SIZE + 1);
            memcpy(g1, g2, R_SIZE + 1);
            memcpy(g2, temp, R_SIZE + 1);
            
            u_deg = poly_degree(u, R_SIZE + 1);
            v_deg = poly_degree(v, R_SIZE + 1);
        }
        
        if (v_deg >= 0) {
            int shift = u_deg - v_deg;
            
            // u = u + v * x^shift
            for (int i = 0; i <= v_deg; i++) {
                if (get_bit(v, i)) {
                    int pos = i + shift;
                    if (get_bit(u, pos)) {
                        clear_bit(u, pos);
                    } else {
                        set_bit(u, pos);
                    }
                }
            }
            
            // g1 = g1 + g2 * x^shift
            for (int i = 0; i < R_SIZE + 1; i++) {
                if (get_bit(g2, i)) {
                    int pos = i + shift;
                    if (pos < (R_SIZE + 1) * 8) {
                        if (get_bit(g1, pos)) {
                            clear_bit(g1, pos);
                        } else {
                            set_bit(g1, pos);
                        }
                    }
                }
            }
        }
    }
    
    // Result is in g1, but we need to reduce it modulo the original modulus
    // Copy lower R_SIZE bytes as result
    memcpy(res_bin, g1, R_SIZE);
}

// Split polynomial e into e0 (lower R_BITS) and e1 (upper bits)
void ntl_split_polynomial(uint8_t e0[R_SIZE],
                         uint8_t e1[R_SIZE],
                         const uint8_t e[2*R_SIZE]) {
    // Clear output arrays
    poly_zero(e0, R_SIZE);
    poly_zero(e1, R_SIZE);
    
    // e0 = lower R_BITS of e
    for (int i = 0; i < R_BITS; i++) {
        if (get_bit(e, i)) {
            set_bit(e0, i);
        }
    }
    
    // e1 = upper bits of e, shifted down by R_BITS
    for (int i = R_BITS; i < 2 * R_BITS && i < 2 * R_SIZE * 8; i++) {
        if (get_bit(e, i)) {
            set_bit(e1, i - R_BITS);
        }
    }
}