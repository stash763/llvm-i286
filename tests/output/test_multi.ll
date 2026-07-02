define i16 @multi_test(i16 %a, i16 %b, i16 %c) {
entry:
  %ab = add i16 %a, %b
  %result = sub i16 %ab, %c
  ret i16 %result
}
