libipf
===============
某Treeofなんとかのipfファイルをなんとかするためのライブラリです。
サンプルとしてunipfが付いています。
Windows版ではGUIのサンプルがあります。

使い方
===============
Windowsの場合はwindows以下のReadmeを見てください。
これ以降はLinux系の話をします。

libipfは以下のパッケージを使用しています。
* g++
* STL(libstdc++)
* zlib

ので、あらかじめインストールしようねそうしましょうね。

Ubuntuの場合は次でOKなはず多分
````
$ sudo apt-get install build-essential
````

````
$ make
````
で、libipf.soとipf unipfができます。


後は、libipf.soを/libか/usr/libにコピーすれば使えます。
コピーしたくない人は、毎回

````
$LD_LIBRARY_PATH=./ ./unipf nanntara
````

とかしないといけません


コマンドライン
--------
コマンドは unipf です。ipfはまだ動きません。
unipf [オプション] ファイル名 [解凍先ディレクトリ]
解凍先が省略された場合は、resultディレクトリを勝手に作ってその中に解凍します。

以下のオプションがあります。

* -h | --help         ヘルプ
* -i | --info         IPFファイルのヘッダ情報などを表示します。半分デバッグ用です。
* -v | --verbose      余計なメッセージを表示しますが現状なにもしてません。

例示
--------
* sample.ipfをtempに解凍
unipf sample.ipf temp


制限とか
--------
今のところ解凍のみです(ライブラリ本体も)。
近いうちにipf作るのに対応したいと思います。

なんたら.iesはなんかしらんけど暗号化されています。つまり二重に暗号化されとるわけやね。
復号するの面倒だったので気が向いたら復号オプションを付けます。


unipf.soの使い方
--------
サンプル見てね以上というのもあれなのでapi.txt見てください。


その他
--------
IPFフォーマット自体はそんなに難しくありません。既存技術の寄せ集めです。
(気になる人はformat.txt見てください)

tos.exeも同じことやってるんやろなと思いながら作りました
でもなんでWindows10で韓国語MFCエラーがでまくるんやろね。
どんなコード組んだらああなるんやろね、IMCやからね、せやね。



TODO
===============
* 圧縮作る
* なんかIPFBrowserみたいなの作ればええね、そやね
* Addon作りやすいの作りたい

謝辞
===============
色々参考にしました。
もともとはIPFUnpackerが元ファイル壊す仕様なのでそれが嫌で作りました。
この場をお借りして多謝。

* IPFBrowser(Excrulon氏) (https://github.com/Excrulon/IPFBrowser)
* IPFUnpacker(r1emu氏) (https://github.com/r1emu/IPFUnpacker)

