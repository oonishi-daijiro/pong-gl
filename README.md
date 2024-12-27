# pong-gl
pong-glはゲーム，pongをOpenGLで実装したものです．
## プレイ方法
Q,Aで左側のパドルを上下に，L,Oで右側のパドルを上下に動かすことができます．  
スペースキーでボールを発射することができます．

## Build(MSYS2)
pacmanを用いて次のパッケージをインストールしておいてください  
mingw-w64-x86_64-glew mingw-w64-x86_64-glfw mingw-w64-x86_64-freetype mingw-w64-x86_64-opencv
```
git clone https://github.com/oonishi-daijiro/pong-gl && cd pong-gl
mkdir build && cd build
cmake .. -G "Ninja" && ninja
./pong right_size_player left_side_player
```

## スクリーンショット
<img src="https://github.com/user-attachments/assets/b05ee1e7-53bd-4751-9743-14778b2b4369" width='50%'>
<img src="https://github.com/user-attachments/assets/13389f19-4771-48a4-ae02-8e01bfff0d93" width='50%'>
<img src="https://github.com/user-attachments/assets/41063ada-99bf-46bf-a325-325dbeb83dfb" width='50%'>


