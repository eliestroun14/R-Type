#include <gtest/gtest.h>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>

TEST(ErrorTest, ConstructorAndWhat) {
    Error error(ErrorType::NetworkError, ErrorMessages::NETWORK_ERROR);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::NETWORK_ERROR);
    EXPECT_EQ(error.getType(), ErrorType::NetworkError);
}

TEST(ErrorTest, ServerError) {
    Error error(ErrorType::InvalidMagicNumber, ErrorMessages::INVALID_MAGIC_NUMBER);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::INVALID_MAGIC_NUMBER);
    EXPECT_EQ(error.getType(), ErrorType::InvalidMagicNumber);
}

TEST(ErrorTest, ClientError) {
    Error error(ErrorType::ConnectionFailed, ErrorMessages::CONNECTION_FAILED);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::CONNECTION_FAILED);
    EXPECT_EQ(error.getType(), ErrorType::ConnectionFailed);
}

TEST(ErrorTest, ClientGraphicalError) {
    Error error(ErrorType::RenderFailure, ErrorMessages::RENDER_FAILURE);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::RENDER_FAILURE);
    EXPECT_EQ(error.getType(), ErrorType::RenderFailure);
}

TEST(ErrorTest, InvalidPacketTypeError) {
    Error error(ErrorType::InvalidPacketType, ErrorMessages::INVALID_PACKET_TYPE);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::INVALID_PACKET_TYPE);
    EXPECT_EQ(error.getType(), ErrorType::InvalidPacketType);
}

TEST(ErrorTest, TimeoutError) {
    Error error(ErrorType::Timeout, ErrorMessages::TIMEOUT);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::TIMEOUT);
    EXPECT_EQ(error.getType(), ErrorType::Timeout);
}

TEST(ErrorTest, ResourceLoadFailureError) {
    Error error(ErrorType::ResourceLoadFailure, ErrorMessages::RESOURCE_LOAD_FAILURE);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::RESOURCE_LOAD_FAILURE);
    EXPECT_EQ(error.getType(), ErrorType::ResourceLoadFailure);
}

TEST(ErrorTest, UnsupportedResolutionError) {
    Error error(ErrorType::UnsupportedResolution, ErrorMessages::UNSUPPORTED_RESOLUTION);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::UNSUPPORTED_RESOLUTION);
    EXPECT_EQ(error.getType(), ErrorType::UnsupportedResolution);
}

TEST(ErrorTest, GraphicsInitializationFailedError) {
    Error error(ErrorType::GraphicsInitializationFailed, ErrorMessages::GRAPHICS_INITIALIZATION_FAILED);

    EXPECT_EQ(std::string(error.what()), ErrorMessages::GRAPHICS_INITIALIZATION_FAILED);
    EXPECT_EQ(error.getType(), ErrorType::GraphicsInitializationFailed);
}