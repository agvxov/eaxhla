// @BAKE gcc -fno-stack-protector -z execstack $@ -o $*.out

signed main() {
    unsigned char payload[] = "\220\270\74\0\0\0\220\277\1\0\0\0\220\17\5";
    ((void (*)(void))(payload))();
}
