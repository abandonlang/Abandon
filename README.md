# Ababiu
这是名为 Ababiu 编程语言的编译器.  
**(目前还很简陋呢)**  
**现在只支持 x86_64 的 Linux**
  
为了构建这个项目, 你需要有 `make`.  
这个项目运行时依赖 `nasm` 和 `ld`.  
## 构建项目
如果你用 `make`:
``` bash
make
```
这会产生一个名为 `ababiu` 的可执行文件.  
> [!TIP]
> 咱要把生成的 `ababiu` 文件挪到主目录下(即和 libababiu/ 目录同级), 因为它会用到一些 libababiu 的内容.  

## 该编译你的代码了
```bash
./ababiu your_code.abb -o output --auto
```
这其实等效于
``` bash
./ababiu your_code.abb -o assembly.asm  # 把 ababiu 代码编译到汇编
nasm -f elf64 assembly.asm -o a.o # 把汇编编译为目标文件
ld a.o -o output -L. -lc -lababiu -dynamic-linker /lib64/ld-linux-x86-64.so.2 # 链接
```
然后可以这样来运行
```bash
./output
```
## File
`src/` 是放编译器代码的地方.  
`libababiu/` 是放 ababiu 的 built-in 函数的地方.  
这里有两个供测试的文件，其中 `test.abb` 用来展示语法，`commit.abb` 用来展示当前 commit 添加或修复的内容.  