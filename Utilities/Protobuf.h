#pragma once

#include <google/protobuf/message.h>

#include <fstream>

namespace ZanaBlocks::Utilities {
/**
 * @brief Saves a Protobuf message to a binary file.
 * @param filename The path to the file where the message will be saved.
 * @param schema The Protobuf message to serialize.
 * @return True if the file was saved successfully, false otherwise.
 */
inline bool saveProtobuf2File(const std::string& filename,
                              const google::protobuf::Message& schema) {
  std::ofstream file(filename, std::ios::binary | std::ios::trunc);
  if (!file.is_open()) {
    return false;
  }
  auto success = schema.SerializeToOstream(&file);
  file.close();

  return success && file.good();
}

/**
 * @brief Loads a Protobuf message from a binary file.
 * * @param filename The path to the file from which the message will be loaded.
 * @param schema The Protobuf message to populate.
 * @return True if the file was loaded and parsed successfully, false otherwise.
 */
inline bool loadProtobufFromFile(const std::string& filename,
                                 google::protobuf::Message& schema) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }
  return schema.ParseFromIstream(&file);
}
}  // namespace ZanaBlocks::Utilities