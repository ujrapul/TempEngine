function(make_xcode TARGET)
  set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_STYLE "")
  set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")
  set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
endfunction()
