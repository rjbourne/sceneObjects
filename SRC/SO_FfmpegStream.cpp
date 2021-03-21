/** \file SO_FfmpegStream.cpp */
#include "sceneObjects.hpp"

#ifdef _WIN32
sceneObjects::SO_FfmpegStream::SO_FfmpegStream(std::string filepathIn) {
    setFilepath(filepathIn);
}

//takes an std::string to set the filepath of the next stream opened
void sceneObjects::SO_FfmpegStream::setFilepath(std::string filepathIn) {
    filepath = filepathIn;
}


//start a stream to an ffmpeg file
void sceneObjects::SO_FfmpegStream::openStream(int widthIn, int heightIn, int fpsIn) {
    //http://blog.mmacklin.com/2013/06/11/real-time-video-capture-with-ffmpeg/
    width = widthIn;
    height = heightIn;
    fps = fpsIn;

    std::string cmd = "ffmpeg -r " + std::to_string(fps) + " -f rawvideo -pix_fmt rgba -s " + std::to_string(width) + "x" + std::to_string(height) + " -i - "
                        "-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " + filepath;

    ffmpeg = _popen(cmd.c_str(), "wb");

    buffer = std::make_unique<int[]>(width*height);

    streaming = true;
}

//render a frame
void sceneObjects::SO_FfmpegStream::renderFrame(void) {
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());
    fwrite(buffer.get(), sizeof(int)*width*height, 1, ffmpeg);
}

void sceneObjects::SO_FfmpegStream::closeStream(void) {
    _pclose(ffmpeg);
}
#endif