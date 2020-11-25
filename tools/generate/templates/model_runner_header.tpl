// This is a TensorFlow Lite model runner interface that has been
// generated using the generate_model_runner tool.

#ifndef {include_guard}
#define {include_guard}

#include <stddef.h>
#include <stdint.h>

typedef struct model_runner_struct model_runner_t;

struct model_runner_struct {{
    void *handle;
}};

#ifdef __cplusplus
extern "C" {{
#endif

/** Initialize the model runner global state.
 *
 * @param[in] arena        Array for scratch and activations.
 * @param[in] arena_size   Size (in bytes) of arena array
 */
void model_runner_init(uint8_t* arena, int arena_size);

/** Create a new model runner for the model content specified.
 *
 * @param[out] ctx             Model runner context
 * @param[in]  model_content   Array containing model content
 */
void model_runner_create(model_runner_t *ctx, const uint8_t* model_content);

/** Get the model input.
 *
 * @param[in] ctx   Model runner context
 * 
 * @return    Pointer to model input buffer. 
 */
int8_t* model_runner_get_input(model_runner_t *ctx);

/** Get the model input size.
 *
 * @param[in] ctx   Model runner context
 * 
 * @return    Model input size (in bytes). 
 */
size_t model_runner_get_input_size(model_runner_t *ctx);


/** Get the model input quantization parameters.
 *
 * @param[in]  ctx    Model runner context
 * @param[out] ctx   Quantization scale
 * @param[out] ctx   Quantization zero point
 */
void model_runner_get_input_quant(model_runner_t *ctx, float *scale, int* zero_point);

/** Run inference using the model runner.
 *
 * @param[in] ctx   Model runner context
 */
void model_runner_invoke(model_runner_t *ctx);

/** Get the model output.
 *
 * @param[in] ctx   Model runner context
 * 
 * @return    Pointer to model output buffer. 
 */
int8_t* model_runner_get_output(model_runner_t *ctx);

/** Get the model output size.
 *
 * @param[in] ctx   Model runner context
 * 
 * @return    Model output size (in bytes). 
 */
size_t model_runner_get_output_size(model_runner_t *ctx);

/** Get the model output quantization parameters.
 *
 * @param[in]  ctx    Model runner context
 * @param[out] ctx   Quantization scale
 * @param[out] ctx   Quantization zero point
 */
void model_runner_get_ouput_quant(float *scale, int* zero_point);

#ifdef __cplusplus
}};
#endif

#endif  // {include_guard}
