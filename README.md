# 概要

Raspberry piでMMAL APIを使うアプリケーションのサンプル。

* test_mmal
	* MMAL APIを使ってカメラからビデオをキャプチャしてファイルに出力する。
	* 出力するファイル名はtest.yuv
	* 出力の形式はYUV(I420)

* test_mmal_enc
	* MMAL APIを使ってtest.yuvをH.264エンコードしてファイルに出力する。
	* 出力するファイル名はtest.h264


# ビルド方法

	$ make test_mmal
	$ make test_mmal_enc

# 使用方法

	$ ./test_mmal
	$ ./test_mmal_enc
	$ MP4Box -fps 15 -add test.h264 -new test.mp4

