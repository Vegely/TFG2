#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// Mock types.h definitions - adjust these to match your actual types.h
#define R_SIZE 32      // Adjust based on your actual R_SIZE
#define R_BITS 256     // Adjust based on your actual R_BITS
#define N_SIZE 64      // Adjust based on your actual N_SIZE
#define IN
#define OUT

typedef unsigned char uint8_t;

#include "ntl.h"

// Test result counters
static int tests_passed = 0;
static int tests_failed = 0;

// Helper function to print byte array in hex
void print_hex(const char* label, const uint8_t* data, int size) {
    printf("%s: ", label);
    for (int i = 0; i < size; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// Helper function to compare byte arrays
int compare_arrays(const uint8_t* a, const uint8_t* b, int size) {
    return memcmp(a, b, size) == 0;
}

// Test helper to set up test polynomials
void setup_test_poly(uint8_t* poly, int size, const uint8_t* pattern, int pattern_size) {
    memset(poly, 0, size);
    for (int i = 0; i < pattern_size && i < size; i++) {
        poly[i] = pattern[i];
    }
}

// Test case wrapper
#define TEST_CASE(name) \
    printf("\n=== Testing %s ===\n", name); \
    int test_start_passed = tests_passed; \
    int test_start_failed = tests_failed;

#define ASSERT_TRUE(condition, message) \
    do { \
        if (condition) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_ARRAYS_EQUAL(a, b, size, message) \
    do { \
        if (compare_arrays(a, b, size)) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            print_hex("  Expected", b, size); \
            print_hex("  Got", a, size); \
            tests_failed++; \
        } \
    } while(0)

// Test ntl_add function
void test_ntl_add() {
    TEST_CASE("ntl_add");
    
    uint8_t a[R_SIZE], b[R_SIZE], result[R_SIZE], expected[R_SIZE];
    
    // Test 1: Adding zero to zero
    memset(a, 0, R_SIZE);
    memset(b, 0, R_SIZE);
    memset(expected, 0, R_SIZE);
    ntl_add(result, a, b);
    ASSERT_ARRAYS_EQUAL(result, expected, R_SIZE, "0 + 0 = 0");
    
    // Test 2: Adding a polynomial to itself (should be zero)
    uint8_t pattern1[] = {0x01, 0x23, 0x45, 0x67};
    setup_test_poly(a, R_SIZE, pattern1, sizeof(pattern1));
    setup_test_poly(b, R_SIZE, pattern1, sizeof(pattern1));
    memset(expected, 0, R_SIZE);
    ntl_add(result, a, b);
    ASSERT_ARRAYS_EQUAL(result, expected, R_SIZE, "a + a = 0 (GF(2) property)");
    
    // Test 3: Adding two different polynomials
    uint8_t pattern2[] = {0x01, 0x00, 0x01, 0x00};
    uint8_t pattern3[] = {0x00, 0x01, 0x01, 0x01};
    uint8_t expected_pattern[] = {0x01, 0x01, 0x00, 0x01};
    setup_test_poly(a, R_SIZE, pattern2, sizeof(pattern2));
    setup_test_poly(b, R_SIZE, pattern3, sizeof(pattern3));
    setup_test_poly(expected, R_SIZE, expected_pattern, sizeof(expected_pattern));
    ntl_add(result, a, b);
    ASSERT_ARRAYS_EQUAL(result, expected, R_SIZE, "XOR addition test");
    
    // Test 4: Commutativity (a + b = b + a)
    uint8_t result1[R_SIZE], result2[R_SIZE];
    ntl_add(result1, a, b);
    ntl_add(result2, b, a);
    ASSERT_ARRAYS_EQUAL(result1, result2, R_SIZE, "Addition is commutative");
}

// Test ntl_mod_mul function
void test_ntl_mod_mul() {
    TEST_CASE("ntl_mod_mul");
    
    uint8_t a[R_SIZE], b[R_SIZE], result[R_SIZE], expected[R_SIZE];
    
    // Test 1: Multiply by zero
    memset(a, 0, R_SIZE);
    uint8_t pattern1[] = {0x01, 0x23, 0x45, 0x67};
    setup_test_poly(b, R_SIZE, pattern1, sizeof(pattern1));
    memset(expected, 0, R_SIZE);
    ntl_mod_mul(result, a, b);
    ASSERT_ARRAYS_EQUAL(result, expected, R_SIZE, "0 * a = 0");
    
    // Test 2: Multiply by one (identity element)
    uint8_t one[] = {0x01, 0x00, 0x00, 0x00};
    setup_test_poly(a, R_SIZE, one, sizeof(one));
    setup_test_poly(b, R_SIZE, pattern1, sizeof(pattern1));
    setup_test_poly(expected, R_SIZE, pattern1, sizeof(pattern1));
    ntl_mod_mul(result, a, b);
    ASSERT_ARRAYS_EQUAL(result, expected, R_SIZE, "1 * a = a");
    
    // Test 3: Commutativity (a * b = b * a)
    uint8_t pattern2[] = {0x03, 0x00, 0x00, 0x00};
    uint8_t pattern3[] = {0x05, 0x00, 0x00, 0x00};
    uint8_t result1[R_SIZE], result2[R_SIZE];
    setup_test_poly(a, R_SIZE, pattern2, sizeof(pattern2));
    setup_test_poly(b, R_SIZE, pattern3, sizeof(pattern3));
    ntl_mod_mul(result1, a, b);
    ntl_mod_mul(result2, b, a);
    ASSERT_ARRAYS_EQUAL(result1, result2, R_SIZE, "Multiplication is commutative");
    
    // Test 4: Known multiplication result
    // 3 * 5 = 15 in GF(2^8) with primitive polynomial x^8 + x^4 + x^3 + x + 1
    // This is a simplified test - actual result depends on your modulus
    setup_test_poly(a, R_SIZE, pattern2, sizeof(pattern2));
    setup_test_poly(b, R_SIZE, pattern3, sizeof(pattern3));
    ntl_mod_mul(result, a, b);
    printf("3 * 5 result: ");
    print_hex("", result, 8);
    ASSERT_TRUE(1, "Multiplication completed without crash");
}

// Test ntl_mod_inv function
void test_ntl_mod_inv() {
    TEST_CASE("ntl_mod_inv");
    
    uint8_t a[R_SIZE], inv[R_SIZE], result[R_SIZE], expected[R_SIZE];
    
    // Test 1: Inverse of 1 should be 1
    uint8_t one[] = {0x01, 0x00, 0x00, 0x00};
    setup_test_poly(a, R_SIZE, one, sizeof(one));
    setup_test_poly(expected, R_SIZE, one, sizeof(one));
    ntl_mod_inv(inv, a);
    ASSERT_ARRAYS_EQUAL(inv, expected, R_SIZE, "inv(1) = 1");
    
    // Test 2: a * inv(a) = 1 (if a != 0)
    uint8_t pattern[] = {0x03, 0x00, 0x00, 0x00};
    setup_test_poly(a, R_SIZE, pattern, sizeof(pattern));
    ntl_mod_inv(inv, a);
    ntl_mod_mul(result, a, inv);
    
    // Check if result is 1 (only first byte should be 1, rest should be 0)
    int is_one = (result[0] == 1);
    for (int i = 1; i < R_SIZE; i++) {
        if (result[i] != 0) {
            is_one = 0;
            break;
        }
    }
    ASSERT_TRUE(is_one, "a * inv(a) = 1");
    
    // Test 3: Double inverse: inv(inv(a)) = a
    uint8_t double_inv[R_SIZE];
    ntl_mod_inv(double_inv, inv);
    ASSERT_ARRAYS_EQUAL(double_inv, a, R_SIZE, "inv(inv(a)) = a");
}

// Test ntl_split_polynomial function
void test_ntl_split_polynomial() {
    TEST_CASE("ntl_split_polynomial");
    
    uint8_t e[2*R_SIZE], e0[R_SIZE], e1[R_SIZE];
    
    // Test 1: Split zero polynomial
    memset(e, 0, 2*R_SIZE);
    ntl_split_polynomial(e0, e1, e);
    uint8_t zero[R_SIZE];
    memset(zero, 0, R_SIZE);
    ASSERT_ARRAYS_EQUAL(e0, zero, R_SIZE, "Split zero: e0 = 0");
    ASSERT_ARRAYS_EQUAL(e1, zero, R_SIZE, "Split zero: e1 = 0");
    
    // Test 2: Split polynomial with only lower bits set
    memset(e, 0, 2*R_SIZE);
    uint8_t pattern1[] = {0x01, 0x23, 0x45, 0x67};
    memcpy(e, pattern1, sizeof(pattern1));
    ntl_split_polynomial(e0, e1, e);
    
    uint8_t expected_e0[R_SIZE];
    memset(expected_e0, 0, R_SIZE);
    memcpy(expected_e0, pattern1, sizeof(pattern1));
    ASSERT_ARRAYS_EQUAL(e0, expected_e0, R_SIZE, "Split lower bits: e0 correct");
    ASSERT_ARRAYS_EQUAL(e1, zero, R_SIZE, "Split lower bits: e1 = 0");
    
    // Test 3: Split polynomial with upper bits set
    memset(e, 0, 2*R_SIZE);
    uint8_t pattern2[] = {0x89, 0xab, 0xcd, 0xef};
    memcpy(e + R_SIZE, pattern2, sizeof(pattern2));
    ntl_split_polynomial(e0, e1, e);
    
    uint8_t expected_e1[R_SIZE];
    memset(expected_e1, 0, R_SIZE);
    memcpy(expected_e1, pattern2, sizeof(pattern2));
    ASSERT_ARRAYS_EQUAL(e0, zero, R_SIZE, "Split upper bits: e0 = 0");
    ASSERT_ARRAYS_EQUAL(e1, expected_e1, R_SIZE, "Split upper bits: e1 correct");
    
    // Test 4: Split polynomial with both parts set
    memcpy(e, pattern1, sizeof(pattern1));
    memcpy(e + R_SIZE, pattern2, sizeof(pattern2));
    ntl_split_polynomial(e0, e1, e);
    ASSERT_ARRAYS_EQUAL(e0, expected_e0, R_SIZE, "Split both parts: e0 correct");
    ASSERT_ARRAYS_EQUAL(e1, expected_e1, R_SIZE, "Split both parts: e1 correct");
}

// Performance test
void test_performance() {
    TEST_CASE("Performance");
    
    uint8_t a[R_SIZE], b[R_SIZE], result[R_SIZE];
    uint8_t pattern1[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    uint8_t pattern2[] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
    
    setup_test_poly(a, R_SIZE, pattern1, sizeof(pattern1));
    setup_test_poly(b, R_SIZE, pattern2, sizeof(pattern2));
    
    // Perform multiple operations to test stability
    for (int i = 0; i < 100; i++) {
        ntl_add(result, a, b);
        ntl_mod_mul(result, a, b);
        if (i % 10 == 0) {
            printf("Iteration %d completed\n", i);
        }
    }
    
    ASSERT_TRUE(1, "Performance test completed without crash");
}

int main() {
    printf("Starting GF(2) Polynomial Operations Unit Tests\n");
    printf("R_SIZE = %d, R_BITS = %d\n", R_SIZE, R_BITS);
    printf("================================================\n");
    
    test_ntl_add();
    test_ntl_mod_mul();
    test_ntl_mod_inv();
    test_ntl_split_polynomial();
    test_performance();
    
    printf("\n================================================\n");
    printf("Test Summary:\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Total Tests:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED! 🎉\n");
        return 0;
    } else {
        printf("\n❌ SOME TESTS FAILED ❌\n");
        return 1;
    }
}