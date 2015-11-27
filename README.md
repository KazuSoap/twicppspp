# twicpps++
このライブラリは twicpps (http://www.soramimi.jp/twicpps/) のフォークです。
  http://www.soramimi.jp/profile.html の利用条件( 以下参照 )を守る必要があります。
* 修正 BSD ライセンス
* ソースコードの配布には著作権表示が必要
* コンパイル済み実行形式の配布には宣伝条項を要求しない
* 他のライセンスを採用する場合、作品毎にその旨を記載
* 外部ライブラリ等を利用する場合、原則としてそれらのライセンスを継承
* 本ライブラリのソースコードを改変 & 配布する場合、全貢献者の著作権を表示
* 他者の作品と偽って配布することの禁止

以下の変更を加えています。
* 付属の改変された liboauth を標準のもの (https://github.com/x42/liboauth) に変更しそのソースコードを削除
    * 自分の環境では、付属の liboauth だと segmentation fault が発生した
    * liboauth のライセンス対応を回避
* 画像付き tweet への対応
    * media/upload (https://dev.twitter.com/rest/reference/post/media/upload) を利用
    * OpenCV との連携 ( cv::Mat 画像を png フォーマットで投稿可能 )
