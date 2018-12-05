#include "ping.h"

APPLICATION_START_WITH(Main $in TonightMode)

int Main(string ARRAY args)
{
	Writer screen = new Writer(Tonight.Std.Console.Output);
	Scanner read = new Scanner(Tonight.Std.File.Input);
	Painter paint = new Painter(Tonight.Resources.Color);
	
	string ipFile = "ping.txt";
	string configFile = "config.txt";
	struct Locale locale;
	
	using(locale $as Tonight.Locale)
	{
		locale.setName("");
		locale.set();
	}
	
	while(true)
	{
		int sleep = 10;
		
		try
		{
			file f = $Empty(file);
			int timeout = 1000;
			
			using(f $in File.open(configFile, File.Mode.read))
			{
				while(!File.end(f))
				{
					string l = read.nextLine(f);
					string ARRAY values = String.split(l, ": ");
					
					if(!String.compare(values[0], "timeout"))
					{
						timeout = Tonight.Convert.toInt(values[1]);
					}
					
					if(!String.compare(values[0], "sleep"))
					{
						sleep = Tonight.Convert.toInt(values[1]);
					}
					
					Array.free(values);
					String.free(l);
				}
				
				File.close(f);
			}
			
			using(f $in File.open(ipFile, File.Mode.read))
			{
				while(!File.end(f))
				{
					int i;
					string l = read.nextLine(f);
					string ARRAY values = String.split(l, ": ");
					string ip = $Empty(string);
					string name = $Empty(string);
					
					forindex(i $in values)
					{
						if(!String.compare(values[i], "ip"))
						{
							ip = values[i + 1];
						}
						
						if(!String.compare(values[i], "name"))
						{
							name = values[i + 1];
						}
					}
					
					ping_t* ret = ping(ip, timeout);
					
					paint.text(7);
					screen.print(name, " (", ip, ") - ", $end);
					
					if(ret)
					{
						paint.text(2);
						screen.println("Tempo de resposta: ", $i(ret->RoundTripTime), " ms", $end);
						Memory.free(ret);
					}
					else
					{
						paint.text(4);
						screen.println("\aErro ao executar função ping(): ", $i(GetLastError()), $end);
					}
					
					String.free(l);
				}
				
				File.close(f);
			}
		}
		catch(GenericException)
		{
			Exception e = getException();
			screen.printargln(Error(e), Message(e), $end);
		}
		
		Tonight.sleep(sleep * 1000);
		Tonight.clearScreen();
	}
	
	return 0;
}
