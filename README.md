libipf
===============
This is a C++ library and set of helper tools for manipulating .ipf files from Tree of Savior. In order to build the library, you will need GNU Make, a C++ compiler and zlib installed.

* To pack a directory named `foo` into a file `bar.ipf`, run `./ipf bar.ipf foo`

libipf (original README)
===============
某Treeofなんとかのipfファイルをなんとかするためのライブラリ＆ツールです。

サンプルとして復号＆解凍を行うunipfとIPFファイル作成＆暗号化ツールのipfが付いています。

Windows版ではこれらのGUI版のサンプルがあります。

Windows向けのツールだけ使いたいっていう人は、ここを見てください。

https://github.com/analysisjp/libipf/releases/tag/v1.00

手っ取り早くWindows用の実行ファイルがほしい人はこちらへ

https://github.com/analysisjp/libipf/releases/download/v1.00/ipf_pack_unipf_1.00.zip

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

[IPFを解凍する場合]

解凍用コマンドは unipf です。

以下凡例

unipf [オプション] ファイル名 [解凍先ディレクトリ]

解凍先が省略された場合は、resultディレクトリを勝手に作ってその中に解凍します。

以下のオプションがあります。

* -h | --help         ヘルプ
* -f | --fhead        IPFファイルのヘッダ情報を表示します。リビジョン番号を調べるのに使います。
* -i | --info         IPFファイル内のテーブル情報を表示します。
* -d | --dryrun       IPFファイルを解凍しません。-fや-iで情報だけ表示したいときに使います。
* -v | --verbose      余計なメッセージを表示しますが現状なにもしてません。

例示
--------
* sample.ipfをtempに解凍


````
$LD_LIBRARY_PATH=./ ./unipf sample.ipf temp
````

* sample.ipfのヘッダ情報だけを表示、解凍はしない


````
$LD_LIBRARY_PATH=./ ./unipf -f -d sample.ipf
````


[IPFを作成する場合]


圧縮用コマンドは ipf です。

以下凡例

ipf [オプション] ファイル名 圧縮するディレクトリ

圧縮先は省略できません。また最低でも


```` 
directory / --- addon.ipf 
            |      |
            |      +---- sysmenu --+--- sysmenu.lua
            |
            --- jp.ipf
                   |
````

のように、最初にxx.ipfのようなディレクトリができていないといけません。

以下のオプションがあります。

* -h | --help         ヘルプ
* -r | --revision num IPFファイルのヘッダ情報にリビジョン番号numを書きます
* -b | --baserev num  IPFファイルのヘッダ情報にベースリビジョン番号numを書きます
* -c | --clevel num   IPFファイル内の各ファイルの圧縮レベルを設定します(0～9)
* -i | --info         圧縮後にIPFファイル内のテーブル情報を表示します。
* -v | --verbose      余計なメッセージを表示しますが現状なにもしてません。

例示
--------
* ディレクトリsampleの内容を sample.ipfに圧縮

````
$LD_LIBRARY_PATH=./ ./ipf sample.ipf sample
````

* ディレクトリsampleの内容を sample.ipfに圧縮。ベースリビジョンは134515、リビジョンは136528に指定


````
$LD_LIBRARY_PATH=./ ./ipf -b 134515 -r 136528 sample.ipf sample
````



制限とか
--------
なんたら.iesはなんかしらんけど暗号化されています。つまり二重に暗号化されとるわけやね。

復号するの面倒だったので気が向いたら復号オプションを付けます。

あるIPFを解凍して圧縮して見比べるとサイズが違う場合が多々あります。

多分これはZipアルゴリズム(か、ハフマンテーブル)が違うからだと思われます。

解凍して個々のファイルを比較しても違いはないので、害はないと思いますし、アドオン作るためとかだったらまあええんですが

元からあるIPFの差し替えをする場合は一応バックアップは取りましょう。クライアント壊れても責任は持てません。


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
* なんかIPFBrowserみたいなの作ればええね、そやね
* Addon作りやすいの作りたい

謝辞
===============
色々参考にしました。

もともとはIPFUnpackerが元ファイル壊す仕様なのでそれが嫌で作りました。

この場をお借りして多謝。

* IPFBrowser(Excrulon氏) (https://github.com/Excrulon/IPFBrowser)
* IPFUnpacker(r1emu氏) (https://github.com/r1emu/IPFUnpacker)

