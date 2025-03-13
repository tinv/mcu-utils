/*
* Copyright (c) 2025 TecInvent Electronics Ltd
*/

#include "adc/mu_adc.h"
#include <errno.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/adc.h>


#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mu_analog);

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx)   \
        ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

static const mu_adc_in_config_t* _config = NULL;
static size_t _configSize = 0;

static int _check_config(void)
{
	for (int i = 0; i < _configSize; i++) {

		if (_config[i].channel >= ARRAY_SIZE(adc_channels)) {
			return -EINVAL;
		}
	}

	return 0;
}

/**!
 *
 * @param value
 * @param data
 * @return
 */
int32_t mu_adc_conv_none(uint16_t value, const mu_adc_conv_data_t* data)
{
	UNUSED(data);
	return value;
}

/**!
 *
 * @param value
 * @param data
 * @return
 */
int32_t mu_adc_conv_linear(uint16_t value, const mu_adc_conv_data_t* data)
{
	return (1.0 * value * data->linear.mult) / (1.0 * data->linear.div) + data->linear.offset;
}

/**!
 *
 * @param value
 * @param data
 * @return
 */
int32_t mu_adc_conv_abs(uint16_t value, const mu_adc_conv_data_t* data)
{
	int32_t out = value + data->linear.offset;
	if (out < 0) {
		out = -out;
	}
	return (1.0 * out * data->linear.mult) / (1.0 * data->linear.div);
}

/**!
 *
 * @param value
 * @param data
 * @return
 */
int32_t mu_adc_conv_table(uint16_t value, const mu_adc_conv_data_t* data)
{
	int i = data->table.size;
	const mu_adc_table_entry_t* t = data->table.table;
	while(--i) {
		if (value < t[i].in) {
			break;
		}
	}
	return t[i].out + data->table.offset;
}

/**!
 *
 * @param config
 * @param configSize
 * @return
 */
static int muAna_init(const mu_adc_in_config_t* config, size_t configSize)
{
	__ASSERT_NO_MSG(config);
	int err = 0;
	_config = config;
	_configSize = configSize;

	err = _check_config();

	if (err < 0) {
		LOG_ERR("invalid configuration");
		return err;
	}

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			LOG_ERR("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return -EIO;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
			return -EIO;
		}
	}

	return 0;
}

static int muAna_measure( const int channel, uint32_t *value )
{
	int ret;
	int32_t val_mv;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {

		if (i == channel) {

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			ret = adc_read_dt(&adc_channels[i], &sequence);
			if (ret < 0) {
				LOG_ERR("Could not read (%d)\n", ret);
				return ret;
			}

			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}

			ret = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);

			if (ret < 0) {
				LOG_ERR("Value in mV not available (err %d)", ret);
				return -EINVAL;
			}

			for (int j = 0u; j < _configSize; j++) {

				if (_config[j].channel == channel) {
					*value = _config[j].conv.fn(val_mv, &_config[j].conv.data);
					return 0;
				}

			}

			return -EINVAL;
		}
	}

	return -ENXIO;
}

const struct mu_adc_if muAdc = {
	.init = muAna_init,
	.measure = muAna_measure
};
