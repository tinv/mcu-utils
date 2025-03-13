/*
* Copyright (c) 2025 TecInvent Electronics Ltd
*/

#ifndef MU_ADC_H_
#define MU_ADC_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint32_t in;
	int32_t out;
} mu_adc_table_entry_t;

typedef union
{
	struct {
		int32_t offset;
		int32_t mult;
		int32_t div;
	} linear;
	struct {
		const mu_adc_table_entry_t* table;
		uint32_t size;
		int32_t offset;
	} table;
} mu_adc_conv_data_t;

typedef struct
{
	int32_t (*fn)(uint16_t value, const mu_adc_conv_data_t* data);
	mu_adc_conv_data_t data;
} mu_adc_in_conv_t;

typedef struct {
	uint32_t channel;
	mu_adc_in_conv_t conv;
} mu_adc_in_config_t;

int32_t mu_adc_conv_none(uint16_t value, const mu_adc_conv_data_t* data);
int32_t mu_adc_conv_linear(uint16_t value, const mu_adc_conv_data_t* data);
int32_t mu_adc_conv_abs(uint16_t value, const mu_adc_conv_data_t* data);
int32_t mu_adc_conv_table(uint16_t value, const mu_adc_conv_data_t* data);

#define MU_ADC_CONV_NONE {				\
	.fn = mu_adc_conv_none,				\
	.data = {					\
		.linear = {               		\
			.offset = 0,			\
			.mult = 1,			\
			.div = 1			\
		}					\
	}	                        		\
},

#define MU_ADC_CONV_LINEAR(o, m, d) {			\
	.fn = mu_adc_conv_linear,	              	\
	.data = {					\
		.linear = {				\
			.offset =  o,			\
			.mult = m,			\
				.div = d		\
		}					\
	}						\
},

#define MU_ADC_CONV_ABS(o, m, d) {			\
	.fn = mu_adc_conv_abs,				\
	.data = {					\
		.simple = {				\
			.offset = o,			\
			.mult = m,                      \
			.div = d                        \
		}                                  	\
	}                                   		\
}
#define MU_ADC_CONV_TABLE(t, o) {			\
	.fn = mu_adc_conv_table,          		\
	.data = {					\
		.table = {				\
			.table = t,			\
			.size = sizeof(t)/sizeof(t[0]),	\
			.offset = o                     \
		}					\
	}						\
},


struct mu_adc_if {

	/**!
	 * @retval 0 on success
	 * @retval For other values refert to errno
	 */
	int (*init)(const mu_adc_in_config_t* config, size_t configSize);

	/**!
	 * Perform measurement on @p channel and retrieve value in mV stored in @p value
	 * @param channel
	 * @param value
	 * @return
	 */
	int (*measure)( const int channel, uint32_t *value );

};

extern const struct mu_adc_if muAdc;

#ifdef __cplusplus
}
#endif

#endif /* MU_ADC_H_ */

