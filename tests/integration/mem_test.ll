define i32 @mem_test() {
entry:
  %a = alloca i32
  store i32 42, ptr %a
  %val = load i32, ptr %a
  ret i32 %val
}
