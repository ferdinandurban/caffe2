#include "caffe2/image/image_input_op.h"

namespace caffe2 {

REGISTER_CPU_OPERATOR(ImageInput, ImageInputOp<CPUContext>);

OPERATOR_SCHEMA(ImageInput)
    .NumInputs(0, 1)
    .NumOutputs(2)
    .TensorInferenceFunction(
        [](const OperatorDef& def, const vector<TensorShape>& /* unused */ ) {
          vector<TensorShape> out(2);
          ArgumentHelper helper(def);
          int batch_size = helper.GetSingleArgument<int>("batch_size", 0);
          int crop = helper.GetSingleArgument<int>("crop", -1);
          int color = helper.GetSingleArgument<int>("color", 1);
          CHECK_GT(crop, 0);
          out[0] = CreateTensorShape(
              vector<int>{batch_size, crop, crop, color ? 3 : 1},
              TensorProto::FLOAT);
          out[1] =
              CreateTensorShape(vector<int>{1, batch_size}, TensorProto::INT32);
          return out;
        })
    .SetDoc(R"DOC(
Imports and processes images from a database. For each run of the operator,
batch_size images will be processed. GPUs can optionally be used for
part of the processing.

The following transformations are applied to the image
  - A bounding box is applied to the initial image (optional)
  - The image is rescaled either up or down (with the scale argument) or
    just up (with the minsize argument)
  - The image is randomly cropped (crop size is passed as an argument but
    the location of the crop is random except if is_test is passed in which case
    the image in cropped at the center)
  - The image is normalized. Each of its color channels can have separate
    normalization values

The dimension of the output image will always be cropxcrop
)DOC")
    .Arg("batch_size", "Number of images to output for each run of the operator"
         ". Must be 1 or greater")
    .Arg("color", "Number of color channels (1 or 3). Defaults to 1")
    .Arg("scale", "Scale the size of the smallest dimension of the image to"
         " this. Scale and minsize are mutually exclusive."
         " Must be larger than crop")
    .Arg("minsize", "Scale the size of the smallest dimension of the image to"
         " this only if the size is initially smaller. Scale and minsize are"
         " mutually exclusive. Must be larger than crop.")
    .Arg("warp", "If 1, both dimensions of the image will be set to minsize or"
         " scale; otherwise, the other dimension is proportionally scaled."
         " Defaults to 0")
    .Arg("crop", "Size to crop the image to. Must be provided")
    .Arg("mirror", "Whether or not to mirror the image. Defaults to 0")
    .Arg("mean", "Mean by which to normalize color channels."
         " Defaults to 0.")
    .Arg("mean_per_channel", "Vector of means per color channel "
         " (1 or 3 elements). Defaults to mean argument. Channel order BGR")
    .Arg("std", "Standard deviation by which to normalize color channels."
         " Defaults to 1.")
    .Arg("std_per_channel", "Vector of standard dev. per color channel "
     " (1 or 3 elements). Defaults to std argument. Channel order is BGR")
    .Arg("bounding_ymin", "Bounding box coordinate. Defaults to -1 (none)")
    .Arg("bounding_xmin", "Bounding box coordinate. Defaults to -1 (none)")
    .Arg("bounding_height", "Bounding box coordinate. Defaults to -1 (none)")
    .Arg("bounding_width", "Bounding box coordinate. Defaults to -1 (none)")
    .Arg("is_test", "Set to 1 to do deterministic cropping. Defaults to 0")
    .Arg("use_caffe_datum", "1 if the input is in Caffe format. Defaults to 0")
    .Arg("use_gpu_transform", "1 if GPU acceleration should be used."
         " Defaults to 0. Can only be 1 in a CUDAContext")
    .Arg("decode_threads", "Number of CPU decode/transform threads."
         " Defaults to 4")
    .Arg("output_type", "If gpu_transform, can set to FLOAT or FLOAT16.")
    .Arg("db", "Name of the database (if not passed as input)")
    .Arg("db_type", "Type of database (if not passed as input)."
         " Defaults to leveldb")
    .Input(0, "reader", "The input reader (a db::DBReader)")
    .Output(0, "data", "Tensor containing the images")
    .Output(1, "label", "Tensor containing the labels");

NO_GRADIENT(ImageInput);

}  // namespace caffe2
