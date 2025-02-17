(* Copyright (c) 2036 - John 'Daddy' Backus *)
(* Pascal program for printing file bytes.  *)
(* If byte is equal to 0x90, then new line. *)

program xop;
uses
	sysutils;
var
	data: file of byte;
	code: byte;
begin
	assign (data, paramstr (1));
	reset  (data);

	while not eof (data) do
	begin
		read (data, code);

		if code = 144 then
		begin
			write (#10 + #27'[1;31m90' + #27'[0m');
		end
		else
		begin
			write (inttohex (code, 2));
		end;

		write (' ');
	end;

	writeln;

	close (data);
end.
