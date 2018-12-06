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
		
		bool echoPing = true;
		bool echoLost = true;
		bool echoTime = true;
		bool echoStatus = true;
		
		int arr_status[] = {0, 0, 0};
		
		int timeout = 1000;
		int warning = 500;
		double tolerance = 0.5;
		
		int netPackLost = $Empty(int);
		double netRespTime = $Empty(double);
		double netLostProp = $Empty(double);
		
		try
		{
			file f = $Empty(file);
			int i = $Empty(int);
			
			using(f $as File.open(configFile, File.Mode.read) $with File.close)
			{
				while(!File.end(f))
				{
					string l = NULL;
					string ARRAY values = NULL;
					
					using(l $as read.nextLine(f) $with String.free)
					{
						using(values $as String.split(l, ": ") $with Array.free)
						{
							Conversor convert = $Empty(Conversor);
							
							using(convert $as Tonight.Convert)
							{
								if(!String.compare(values[0], "timeout"))
								{
									timeout = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "sleep"))
								{
									sleep = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "repeat"))
								{
									repeat = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "warning"))
								{
									warning = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "tolerance"))
								{
									tolerance = 1.0 - (convert.toDouble(values[1]) / 100.0);
								}
								
								if(!String.compare(values[0], "echo-ping"))
								{
									echoPing = convert.toBool(values[1]);
								}
								
								if(!String.compare(values[0], "echo-lost"))
								{
									echoLost = convert.toBool(values[1]);
								}
								
								if(!String.compare(values[0], "echo-time"))
								{
									echoTime = convert.toBool(values[1]);
								}
								
								if(!String.compare(values[0], "echo-status"))
								{
									echoStatus = convert.toBool(values[1]);
								}
							}
						}
					}
				}
			}
			
			using(f $as File.open(ipFile, File.Mode.read) $with File.close)
			{
				while(!File.end(f))
				{
					int i, lost = $Empty(int);
					int status = 0;
					double media = $Empty(double);
					string l = $Empty(string);
					
					using(l $as read.nextLine(f) $with String.free)
					{
						string ARRAY values = NULL;
						
						using(values $as String.split(l, ": ") $with Array.free)
						{
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
							
							if(echoPing)
							{
								screen.textln("\tDisparo de pacotes:");
							}
							
							for(i = 0; i < repeat; i++)
							{
								ping_t* ret = NULL;
								
								using(ret $as ping(ip, timeout) $with Memory.free)
								{
									if(echoPing)
									{
										paint.text(7);
										screen.print("\t", $i(i + 1), ": ", $end);
									}
									
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
										
										if(echoPing)
										{
											screen.print(
												$i(ret->RoundTripTime),
												" ms",
												$end
											);
										}
										
										media += ret->RoundTripTime;
									}
									else
									{
										if(echoPing)
										{
											paint.text(4);
											screen.text("Perdido");
										}
										lost++;
									}
								}
							}
						}
					}
					
					if(echoPing)
					{
						screen.nl();
					}
					
					paint.text(7);
					
					if(echoLost)
					{
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
					}
					
					if(echoTime)
					{
						paint.text(7);
						screen.text("\tTempo médio de resposta: ");
					}
					
					media /= repeat;
					
					if(media < warning)
					{
						paint.text(2);
					}
					else
					{
						paint.text(6);
					}
					
					if(echoTime)
					{
						screen.println($d(media), " ms", $end);
					}
					
					if(lost < tolerance * repeat)
					{
						status += 2;
					}
					
					if(media < warning)
					{
						status++;
					}
					
					if(status >= 2)
					{
						netRespTime += media;
						netPackLost += lost;
					}
					
					if(echoStatus)
					{
						paint.text(7);
						screen.text("\tStatus: ");
					}
					
					switch(status)
					{
						case 0:
						case 1:
							if(echoStatus)
							{
								paint.text(4);
								screen.textln("Inativo");
							}
							arr_status[0]++;
							break;
							
						case 2:
							if(echoStatus)
							{
								paint.text(1);
								screen.textln("Instável");
							}
							arr_status[1]++;
							arr_status[2]++;
							break;
							
						case 3:
							if(echoStatus)
							{
								paint.text(2);
								screen.textln("Ativo");
							}
							arr_status[2]++;
							break;
							
						default:
							if(echoStatus)
							{
								paint.text(6);
								screen.println($i(status), " - Não identificado", $end);
							}
							break;
					}
					
					screen.nl();
				}
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
		screen.text("Perda de pacotes em hosts ativos: ");
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
		
		paint.text(2);
		screen.println("Hosts ativos: ", $i(arr_status[2]), $end);
		paint.text(1);
		screen.println("Hosts instáveis: ", $i(arr_status[1]), $end);
		paint.text(4);
		screen.println("Hosts inativos: ", $i(arr_status[0]), $end);
		
		Tonight.sleep(sleep * 1000);
		Tonight.clearScreen();
	}
	
	return 0;
}
