program one;
type 
	a = record
		b, c: integer;
		d: boolean;
		end;
var
	i: a;
	qw, we, er: integer;
begin
	read(qw, we);
	while (true) do
	begin
		read(i.b);
		if (qw > i.b) then
			er := we + i.b;
	end;
	
	write(er);
end.