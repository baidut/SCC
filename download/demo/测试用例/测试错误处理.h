
int i;
{
	## i= 0;
	read @@ i;
}

{
	int a;
	{@ %
		$if(  a <b  | c >=d)
		then write 1;
	}	
}