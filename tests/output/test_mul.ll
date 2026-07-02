define i32 @mul_test(i16 %a, i16 %b) {
entry:
  %result = mul i16 %a, %b
  ret i16 %result
}
