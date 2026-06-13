#include "w32.h"
#include <stdlib.h>

volatile size_t g_sink = 0;

int main(void) {
    const size_t MB = 1024 * 1024;
    const size_t payload_size = 1024 * MB;
    const int iterations = 10;

    size_t enc_max_len = ((payload_size * 8) / 5) + 64;
    size_t dec_max_len = payload_size + 64;

    char *raw_data = (char*)malloc(payload_size);
    char *enc_data = (char*)malloc(enc_max_len);
    char *dec_data = (char*)malloc(dec_max_len);
    
    size_t s_len, d_len, enc_written;
    size_t i;
    int res;

    if (!raw_data || !enc_data || !dec_data) {
        return 1;
    }

	/* We do not warm the memory inorder to try
	 * mimic real world usage. This will test
	 * stride and prefetch effeciency alongside
	 * the core algorithm itself.
	 */

    for (i = 0; i < iterations; i++) {
        s_len = payload_size;
        d_len = enc_max_len;
        
        res = w32enc(raw_data, &s_len, enc_data, &d_len);
        enc_written = d_len; 
        g_sink += enc_written + res;

        s_len = enc_written;
        d_len = dec_max_len;
        
        res = w32dec(enc_data, &s_len, dec_data, &d_len);
        g_sink += d_len + res;
    }

    free(raw_data);
    free(enc_data);
    free(dec_data);
    
    return (g_sink == 0) ? 1 : 0; 
}
