# ConfFileChgツールについて
HYRON 江
2017.05.09

## 1. ConfFileChgツールの機能
csvファイルを参照して、変更箇所をconfigファイルに自動反映する。

## 2. 使用方法：
0). お客さんから入手したExcelファイルをcsvファイルに変換しておく。
1). 上記0.のcsvファイルを本ConfFileChgディレクトリにコピーしておく。
2). target/usr/mcc/configを丸ごとConfFileChgツールのディレクトリにコピーしておく。
3). VMで./changefile.shを実行して、変更あったconfigファイルを各監視シーンに生成される。
　  変更あったconfigの変更箇所もコンソールに出力される。変更前後の差分もConfFileChg_diff.csvに
　　出力される。
4). ./ConfFileChg/scene_xxxをtarget/usr/mcc/configに上書きコピーして下さい。

## 3. 注意事項：
ツールを実行する前に、下記の条件を満たしているかを確認する必要がある。 
--ワーキングディレクトリ
 |--config(/target/usr/mcc/configからコピーする)
 |--windowsでxlsmから生成したすべての*.csvファイル
 |--ConfFileChgの実行可能ファイル
 |--changefile.sh
 
以上
