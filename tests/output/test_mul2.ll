; Function: mul_test
; Computes a * b where a and b are parameters

define i32 @mul_test(i32 %a, i32 %b) {
entry:
  %result = mul i32 %a, %b
  ret i32 %result
}
