Var
    i : longint;

Begin
    For i:=0 to ParamCount do
        Writeln (i : 3, ': ', ParamStr (i));
End.