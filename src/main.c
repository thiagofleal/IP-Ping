#include <Tonight\tonight.h>
#include <Tonight\list.h>
#include "ping.h"

APPLICATION_START_WITH(Main $in TonightMode $as static)

const struct{
	int danger;
	int warning;
	int regular;
	int good;
	int normal;
}Index = {0, 1, 2, 3, 4};

static int Main(string ARRAY args)
{
	Writer screen = new Writer(Tonight.Std.Console.Output);
	Writer write = new Writer(Tonight.Std.File.Output);
	Scanner read = new Scanner(Tonight.Std.File.Input);
	Painter paint = new Painter(Tonight.Resources.Color);
	Timer now = new Timer(Tonight.Std.TimeNow);
	
	string ipFile = "../config/ping.txt";
	string configFile = "../config/config.txt";
	string logFile = "../log.csv";
	
	object queue = $Empty(object);
	
	Tonight.Locale.setName("");
	Tonight.Locale.set();
	
	try
	{
		using(queue $as new Object(List.class) $with delete)
		{
			$(queue $as List).setFreeCallBack(Memory.free);
			
			while(true)
			{
				int sleep = 10;
				int repeat = 5;
				int size = 5;
				int fallWarning = 3;
				int fallDanger = 5;
				int n = 0, i;
				int count = 0;
				
				bool echoPing = true;
				bool echoLost = true;
				bool echoTime = true;
				bool echoStatus = true;
				
				bool list_status[] = {true, true, true, true};
				bool beeping = false;
				
				int arr_status[] = {0, 0, 0, 0};
				int status_color[] = {4, 1, 6, 2, 7};
				string name_status_s[] = {"Inativo", "Instavel", "Regular", "Bom"};
				string name_status_p[] = {"inativos", "instáveis", "regulares", "bons"};
				
				int timeout = 1000;
				int warning = 5;
				int danger = 10;
				double tolerance = 0.5;
				
				int bgcolor = 0;
				int netPackLost = $Empty(int);
				double netRespTime = $Empty(double);
				double netLostProp = $Empty(double);
				
				try
				{
					object list = NULL;
					pointer ARRAY arr = NULL;
					
					paint.background(bgcolor);
					
					using(list $as new Object(List.class) $with delete)
					{
						file log = $Empty(file);
						file f = $Empty(file);
						int i = $Empty(int);
						
						$(list $as List).setFreeCallBack((P_freeCallBack)free_IP_status);
						
						using(log $as File.open(logFile, File.Mode.append) $with File.close)
						{
							write.print(
								log,
								"Data:;",
								$F("%02i/%02i/%i",
									now.day_month(),
									now.month(),
									now.year()
								),
								";Horário:;",
								$F("%02i:%02i:%02i",
									now.hours(),
									now.minutes(),
									now.seconds()
								),
								";",
								$end
							);
							
							using(f $as File.open(configFile, File.Mode.read) $with File.close)
							{
								while(!File.end(f))
								{
									string l = NULL;
									string ARRAY values = NULL;
									
									using(l $as read.nextLine(f) $with String.free)
									{
										using(values $as String.split(l, ": ") $with freeStringArray)
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
												
												if(!String.compare(values[0], "time-warning"))
												{
													warning = convert.toInt(values[1]);
												}
												
												if(!String.compare(values[0], "time-danger"))
												{
													danger = convert.toInt(values[1]);
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
												
												if(!String.compare(values[0], "list-danger"))
												{
													list_status[Index.danger] = convert.toBool(values[1]);
												}
												
												if(!String.compare(values[0], "list-warning"))
												{
													list_status[Index.warning] = convert.toBool(values[1]);
												}
												
												if(!String.compare(values[0], "list-regular"))
												{
													list_status[Index.regular] = convert.toBool(values[1]);
												}
												
												if(!String.compare(values[0], "list-good"))
												{
													list_status[Index.good] = convert.toBool(values[1]);
												}
												
												if(!String.compare(values[0], "beep"))
												{
													beeping = convert.toBool(values[1]);
												}
												
												if(!String.compare(values[0], "buffer-size"))
												{
													size = convert.toInt(values[1]);
												}
												
												if(!String.compare(values[0], "fall-warning"))
												{
													fallWarning = convert.toInt(values[1]);
												}
												
												if(!String.compare(values[0], "fall-danger"))
												{
													fallDanger = convert.toInt(values[1]);
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
									string ip = $Empty(string);
									string name = $Empty(string);
									
									using(l $as read.nextLine(f) $with String.free)
									{
										string ARRAY values = NULL;
										
										using(values $as String.split(l, ": ") $with freeStringArray)
										{
											n++;
											
											forindex(i $in values)
											{
												if(!String.compare(values[i], "ip"))
												{
													ip = String.copy(values[i + 1]);
												}
												
												if(!String.compare(values[i], "name"))
												{
													name = String.copy(values[i + 1]);
												}
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
													else if(ret->RoundTripTime < danger)
													{
														paint.text(status_color[Index.regular]);
													}
													else
													{
														paint.text(status_color[Index.danger]);
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
														screen.text("-");
													}
													lost++;
												}
											}
										}
									}
									
									if(echoPing)
									{
										screen.nl();
									}
									
									paint.text(status_color[Index.normal]);
									
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
									
									if(media < warning)
									{
										paint.text(status_color[Index.good]);
									}
									else if(media < danger)
									{
										paint.text(status_color[Index.regular]);
									}
									else
									{
										paint.text(status_color[Index.danger]);
									}
									
									if(echoTime)
									{
										if(status >= Index.warning)
										{
											screen.println($d(media), " ms", $end);
										}
										else
										{
											paint.text(status_color[Index.danger]);
											screen.textln("-");
										}
									}
									
									if(status >= Index.warning)
									{
										netRespTime += media;
										netPackLost += lost;
										count++;
									}
									
									$(list $as List).add(new_IP_status(ip, name, status));
									
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
							
							paint.text(status_color[Index.normal]);
							screen.nl();
							screen.textln("Classificação por status:");
							screen.nl();
							
							for(i = ARRAY_LENGTH(arr_status) - 1; i >= 0; i--)
							{
								paint.text(status_color[i]);
								screen.println("\t[", name_status_p[i], "]: ", $i(arr_status[i]), $end);
							}
							
							for(i = ARRAY_LENGTH(arr_status) - 1; i >= 0; i--)
							{
								if(list_status[i])
								{
									pointer ARRAY array = NULL;
									
									paint.text(status_color[Index.normal]);
									screen.nl();
									screen.println("Hosts ", name_status_p[i], ": ", $end);
									write.print(log, "\rHosts ", name_status_p[i], $end);
									screen.nl();
									paint.text(status_color[i]);
									
									using(array $as $(list $as List).toArray() $with Array.free)
									{
										IP_status* ip_s = $Empty(IP_status*);
										
										foreach(ip_s $in array)
										{
											if(ip_s->status == i)
											{
												screen.println("\t", ip_s->name, ": ", ip_s->ip, $end);
												write.println(log, ";", ip_s->name, ";", ip_s->ip, ";", $end);
											}
										}
										
										if(i == Index.danger and Array.length(array))
										{
											wait_beep(beeping);
										}
									}
								}
							}
							
							screen.nl();
							
							netRespTime /= count;
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
							write.println(log, "Tempo médio de resposta (ms):;", $d(netRespTime), ";", $end);
							$(queue $as List).add(new Double(netRespTime));
							
							while($(queue $as List).size() > size)
							{
								$(queue $as List).remove(0);
							}
							
							paint.text(status_color[Index.normal]);
							screen.text("Perda de pacotes em hosts ativos: ");
							netLostProp = ((double)netPackLost / (double)(count * repeat)) * 100.0;
							
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
								$i(count * repeat),
								" (",
								$d(netLostProp),
								"%)",
								$end
							);
							
							using(arr $as $(queue $as List).toArray() $with Array.free)
							{
								int fall = 0, i;
								int color;
								double last = 0;
								double *pd = NULL;
								string status = $Empty(string);
								
								screen.nl();
								paint.text(status_color[Index.normal]);
								screen.textln("Médias anteriores:");
								screen.nl();
								
								foreach(pd $in arr)
								{
									if(*pd < warning)
									{
										paint.text(color = status_color[Index.good]);
									}
									else if(*pd < danger)
									{
										paint.text(color = status_color[Index.regular]);
									}
									else
									{
										paint.text(color = status_color[Index.danger]);
									}
									
									if(last and *pd > last)
									{
										++ fall;
									}
									else if(*pd < last)
									{
										fall = 0;
									}
									
									last = *pd;
									screen.print("\t", $dpf(pd, 3), " ms\t", $end);
									paint.both(bgcolor, color);
									
									for(i = 0; i <= *pd * 3; i++)
									{
										screen.text("-");
									}
									
									paint.background(bgcolor);
									screen.nl();
								}
								
								screen.nl();
								paint.text(status_color[Index.normal]);
								screen.text("Análise de histórico de desempenho: ");
								
								if(fall < fallWarning)
								{
									paint.text(status_color[Index.good]);
									status = "Estável";
								}
								else if(fall < fallDanger)
								{
									paint.text(status_color[Index.regular]);
									status = "Regular";
								}
								else
								{
									paint.text(status_color[Index.danger]);
									status = "Instável";
								}
								
								screen.textln(status);
								write.println(log, "Status da rede:;", status, $end);
							}
						}
					}
				}
				catch(GenericException)
				{
					Exception e = getException();
					paint.text(status_color[Index.danger]);
					screen.printargln(Error(e), Message(e), $end);
				}
				
				Tonight.sleep(sleep * 1000);
				Tonight.clearScreen();
			}
		}
	}
	catch(GenericException)
	{
		Exception e = getException();
		screen.printargln(Error(e), Message(e), $end);
	}
	
	return Exit.Success;
}
