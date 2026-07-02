define i16 @xor_test(i16 %a, i16 %b) {
entry:
  %result = xor i16 %a, %b
  ret i16 %result
}
