#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/version.h"

// our model 
#include "ecgclassifier.h"

#define DEBUG 1

// TFLite globals, used for compatibility with Arduino-style sketches
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;

  // Create an area of memory to use for input, output, and other TensorFlow
  // arrays. You'll need to adjust this by compiling, running, and looking
  // for errors.
  constexpr int kTensorArenaSize = 10 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setup() {
// Wait for Serial to connect
#if DEBUG
  while(!Serial);
#endif

  // Set up logging (will report to Serial, even within TFLite functions)
  static tflite::MicroErrorReporter micro_error_reporter; 
  error_reporter = &micro_error_reporter;

  // Map model into a usable data_structure 
  model = tflite::GetModel(ecgclassifier);

  if (model->version() != TFLITE_SCHEMA_VERSION){
    error_reporter->Report("Model version does not match Schema");
    while(1);
  }



  // Pull in only needed operations (should match NN layers)
  // Available ops:
  // https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/kernels/micro_ops.h
  // static tflite::MicroMutableOpResolver micro_mutable_op_resolver;
  // micro_mutable_op_resolver.AddBuiltin(
  //   tflite::BuiltinOperator_FULLY_CONNECTED,
  //   tflite::ops::micro::Register_FULLY_CONNECTED(),
  //   1,3);

  //  ------- New code -----------
  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;
    //  ------- New code -----------


  // Build an interpreter to run the model -> it will run the inference
static tflite::MicroInterpreter static_interpreter(
    model,
    resolver,
    tensor_arena,
    kTensorArenaSize,
    nullptr,
    nullptr
);

  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors

  TfLiteStatus allocate_status = interpreter->AllocateTensors();

  if (allocate_status != kTfLiteOk){
    error_reporter->Report("Allocate Tensors() failed");
    // If error halt the program
    while(1);
  }

  //  Create handles for input and output buffers
  model_input = interpreter->input(0);
  model_output = interpreter->output(0);

  // Get information about the memory area to use for the model's input
  // Supported data types:
  // https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/c/common.h#L226
#if DEBUG
  Serial.print("Number of dimensions: ");
  Serial.println(model_input->dims->size);
  Serial.println("Dim 1 size: ");
  Serial.println(model_input->dims->data[0]);
  Serial.println("Dim 2 size: ");
  Serial.println(model_input->dims->data[1]);
  Serial.println("Input type: ");
  Serial.println(model_input->type);
#endif

}

void loop() {
#if DEBUG 
  unsigned long start_timestamp = micros();
#endif 

  // Raw data for testing the model
float inputData[] = {
    2.22621545e-01,  2.47012704e-01,  2.76026224e+01,  0.00000000e+00,
    6.00000000e+00,  0.00000000e+00,  1.93110581e-01,  3.22829178e-01,
    2.74391478e-01,  6.77941273e+00,  2.77555756e-17, -1.60853738e-01,
    -1.60853738e-01, -3.37792851e+00,  1.23000000e+02,  2.39321959e+00,
    6.15000000e+00,  6.00000000e+00,  9.06474820e+03
};

  // copy value to feed to the model
for (int i = 0; i < 19; i++) {
    model_input->data.f[i] = inputData[i];
}

  // Run inference

  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on input:\n");
  }
  
  // Read predicted y value from output buffer (tensor)
  float y_val = model_output->data.f[0];
  // Print value
  Serial.println("Y Value is ");
  Serial.println(y_val);

#if DEBUG
  Serial.print("Time for inference (us): ");
  Serial.println(micros() - start_timestamp);
#endif

  delay(1000);

}
