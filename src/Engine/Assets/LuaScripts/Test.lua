-- SPDX-FileCopyrightText: 2023 Ujwal Vujjini
-- SPDX-License-Identifier: MIT

-- print("Hello from Lua")
-- print("Lua code is capable of reading the value set from C++", cppvar)
-- luavar = cppvar * 3

function myluafunction(times)
  -- print("GOT HERE wfe;f vsvnl! safwe ")
  return string.rep("(-)", times)
end

function myfunction(arg)
  return cppfunction(arg)
end
