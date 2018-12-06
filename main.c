#include "ping.h"

APPLICATION_START_WITH(Main $in TonightMode)

const struct{
	int danger;
	int warning;
	int regular;
	int good;
	int normal;
}Index = {0, 1, 2, 3, 4};

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
		int i;
		
		bool echoPing = true;
		bool echoLost = true;
		bool echoTime = true;
		bool echoStatus = true;
		
		int arr_status[] = {0, 0, 0, 0};
		int status_color[] = {4, 1, 6, 2, 7};
		string name_status_s[] = {"Inativo", "Instavel", "Regular", "Bom"};
		string name_status_p[] = {"inativos", "instáveis", "regulares", "bons"};
		
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
								
								if(!String.compare(values[0], "color-danger"))
								{
									status_color[Index.danger] = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "color-warning"))
								{
									status_color[Index.warning] = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "color-regular"))
								{
									status_color[Index.regular] = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "color-good"))
								{
									status_color[Index.good] = convert.toInt(values[1]);
								}
								
								if(!String.compare(values[0], "color-normal"))
								{
									status_color[Index.normal] = convert.toInt(values[1]);
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
							
							paint.text(status_color[Index.normal]);
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
										paint.text(status_color[Index.normal]);
										screen.print("\t", $i(i + 1), ": ", $end);
									}
									
									if(ret)
									{
										if(ret->RoundTripTime < warning)
										{
											paint.text(status_color[Index.good]);
										}
										else
										{
											paint.text(status_color[Index.regular]);
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
											paint.text(status_color[Index.danger]);
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
					
					paint.text(status_color[4]);
					
					if(echoLost)
					{
						screen.text("\tPerdas de pacotes: ");
						
						if(lost == 0)
						{
							paint.text(status_color[Index.good]);
						}
						else if(lost < tolerance * repeat)
						{
							paint.text(status_color[Index.warning]);
							beep(1);
						}
						else
						{
							paint.text(status_color[Index.danger]);
							beep(3);
						}
						
						screen.textln($i(lost));
					}
					
					if(echoTime)
					{
						paint.text(status_color[Index.normal]);
						screen.text("\tTempo médio de resposta: ");
					}
					
					media /= repeat;
					
					if(media < warning)
					{
						paint.text(status_color[Index.good]);
					}
					else
					{
						paint.text(status_color[Index.regular]);
					}
					
					if(echoTime)
					{
						screen.println($d(media), " ms", $end);
					}
					
					if(!lost and media < warning)
					{
						status = Index.good;
					}
					else if(!lost)
					{
						status = Index.regular;
					}
					else if(lost < tolerance * repeat)
					{
						status = Index.warning;
					}
					else
					{
						status = Index.danger;
					}
					
					if(status >= Index.regular)
					{
						netRespTime += media;
						netPackLost += lost;
					}
					
					if(echoStatus)
					{
						paint.text(status_color[Index.normal]);
						screen.text("\tStatus: ");
					}
					
					if(echoStatus)
					{
						paint.text(status_color[status]);
						screen.textln(name_status_s[status]);
					}
					arr_status[status]++;
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
		paint.text(status_color[Index.normal]);
		screen.text("Tempo médio de resposta da rede: ");
		
		if(netRespTime < warning)
		{
			paint.text(status_color[Index.good]);
		}
		else
		{
			paint.text(status_color[Index.regular]);
			beep(1);
		}
		
		screen.println($d(netRespTime), " milisegundos", $end);
		
		paint.text(status_color[Index.normal]);
		screen.print("Perda de pacotes em hosts ", name_status_p[Index.warning], ": ", $end);
		netLostProp = ((double)netPackLost / (double)(n * repeat)) * 100.0;
		
		if(netLostProp == 0)
		{
			paint.text(status_color[Index.good]);
		}
		else if(netLostProp < (1 - tolerance) * 100)
		{
			paint.text(status_color[Index.warning]);
		}
		else
		{
			int i;
			paint.text(status_color[Index.danger]);
			
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
		
		paint.text(status_color[Index.normal]);
		screen.nl();
		screen.textln("Classificação por status:");
		screen.nl();
		
		for(i = (sizeof arr_status / sizeof(int) - 1); i >= 0; i--)
		{
			paint.text(status_color[i]);
			screen.println("\t[", name_status_p[i], "]: ", $i(arr_status[i]), $end);
		}
		
		Tonight.sleep(sleep * 1000);
		Tonight.clearScreen();
	}
	
	return 0;
}
