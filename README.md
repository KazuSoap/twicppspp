# twicpps++
このライブラリは twicpps (http://www.soramimi.jp/twicpps/) のフォークです。

以下の変更を加えています。
* 付属の改変された liboauth を標準のもの (https://github.com/x42/liboauth) に変更しそのソースコードを削除
    * 自分の環境では、付属の liboauth だと segmentation fault が発生した
    * liboauth のライセンス対応を回避
* 画像付き tweet への対応
    * media/upload (https://dev.twitter.com/rest/reference/post/media/upload) を利用
    * OpenCV との連携 ( cv::Mat 画像を png フォーマットで投稿可能 )
