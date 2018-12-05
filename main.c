#include "ping.h"

APPLICATION_START_WITH(Main $in TonightMode)

void beep(int number)
{
	Writer out = new Writer(Tonight.Std.Console.Output);
	int i;
	
	for(i = 0; i < number; i++)
	{
		out.text("\a");
		Tonight.sleep(500);
	}
}

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
		int repeat = 5;
		int n = 0;
		
		int timeout = 1000;
		int warning = 500;
		double tolerance = 0.5;
		
		int netPackLost = $Empty(int);
		double netRespTime = $Empty(double);
		double netLostProp = $Empty(double);
		
		try
		{
			file f = $Empty(file);
			
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
					
					if(!String.compare(values[0], "repeat"))
					{
						repeat = Tonight.Convert.toInt(values[1]);
					}
					
					if(!String.compare(values[0], "warning"))
					{
						warning = Tonight.Convert.toInt(values[1]);
					}
					
					if(!String.compare(values[0], "tolerance"))
					{
						tolerance = 1.0 - (Tonight.Convert.toDouble(values[1]) / 100.0);
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
					int i, lost = $Empty(int);
					double media = $Empty(double);
					
					string l = read.nextLine(f);
					string ARRAY values = String.split(l, ": ");
					string ip = $Empty(string);
					string name = $Empty(string);
					
					n++;
					
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
					
					paint.text(7);
					screen.println(name, " (", ip, ")", $end);
					screen.textln("\tDisparo de pacotes:");
					
					for(i = 0; i < repeat; i++)
					{
						ping_t* ret = ping(ip, timeout);
						
						paint.text(7);
						screen.print("\t", $i(i + 1), ": ", $end);
						
						if(ret)
						{
							if(ret->RoundTripTime < warning)
							{
								paint.text(2);
							}
							else
							{
								paint.text(6);
							}
							
							screen.print(
								$i(ret->RoundTripTime),
								" ms",
								$end
							);
							
							media += ret->RoundTripTime;
							Memory.free(ret);
						}
						else
						{
							paint.text(4);
							screen.text("Perdido");
							lost++;
						}
					}
					
					screen.nl();
					String.free(l);
					
					paint.text(7);
					netPackLost += lost;
					screen.text("\tPerdas de pacotes: ");
					
					if(lost == 0)
					{
						paint.text(2);
					}
					else if(lost < tolerance * repeat)
					{
						paint.text(1);
						beep(1);
					}
					else
					{
						paint.text(4);
						beep(3);
					}
					
					screen.textln($i(lost));
					
					paint.text(7);
					screen.text("\tTempo médio de resposta: ");
					
					media /= repeat;
					netRespTime += media;
					
					if(media < warning)
					{
						paint.text(2);
					}
					else
					{
						paint.text(6);
					}
					
					screen.println($d(media), " ms", $end);
					screen.nl();
				}
				
				File.close(f);
			}
		}
		catch(GenericException)
		{
			Exception e = getException();
			screen.printargln(Error(e), Message(e), $end);
		}
		
		screen.nl();
		
		netRespTime /= n;
		paint.text(7);
		screen.text("Tempo médio de resposta da rede: ");
		
		if(netRespTime < warning)
		{
			paint.text(2);
		}
		else
		{
			paint.text(6);
			beep(1);
		}
		
		screen.println($d(netRespTime), " milisegundos", $end);
		
		paint.text(7);
		screen.text("Perda total de pacotes: ");
		netLostProp = ((double)netPackLost / (double)(n * repeat)) * 100.0;
		
		if(netLostProp == 0)
		{
			paint.text(2);
		}
		else if(netLostProp < (1 - tolerance) * 100)
		{
			paint.text(1);
		}
		else
		{
			int i;
			paint.text(4);
			
			for(i = 0; i < 3; i++)
			{
				beep(3);
				Tonight.sleep(1000);
			}
		}
		
		screen.println(
			$i(netPackLost),
			"/",
			$i(n * repeat),
			" (",
			$d(netLostProp),
			"%)",
			$end
		);
		
		Tonight.sleep(sleep * 1000);
		Tonight.clearScreen();
	}
	
	return 0;
}
