﻿#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <nstd/File.hpp>
#include "core/cmdlib.h"
#include "script.hpp"
#include "mission.hpp"
//----------------------------------------------------

void CompileMissionBlk(const char* filename, 
	MS_MISSION settings, MS_TARGET targets[16], Stack stack, StringsStack strings)
{
	String filename_str = String::fromPrintf("%s", filename);
	File* file = new File();

	if (!file->open(filename_str, File::writeFlag))
	{
		MsgWarning("%s can't be opened\n", filename);
		exit(EXIT_FAILURE);
	}

	file->write(&settings, 0x86);
	file->write(&targets, 1024);
	file->write(&stack.operations, stack.nbOperations);

	//file->write(&strings.strings, strings)
	
	file->close();
}

void CreateMission2(const char* filename)
{
	
}

void CreateMission(const char* filename)
{	

	MsgInfo("%s mission file is opened\n", filename);

	MS_MISSION settings;
	MS_TARGET targets[16];
	StringsStack strings;
	Stack stack;
	initStack(&stack);
	char str[64];
	int value = 0;
	uint nbTargets = 0;
	uint nbThreads = 0;

	settings.id = MISSION_IDENT;
	settings.size = 0x84;
	settings.playerStartPosition = { 0, 0 };
	settings.playerStartRotation = { 0 };
	bool done = false;

	do
	{
		MsgInfo("Which cities do you want create your mission\n");
		MsgInfo("\tChicago: 0\n");
		MsgInfo("\tHavana: 1\n");
		MsgInfo("\tLas Vegas: 2\n");
		MsgInfo("\tRio: 3\n");

		MsgInfo("Enter a number: ");
		scanf("%d", &settings.city);

		if (settings.city >= 0 && settings.city <= 3) done = true;
	} while (!done);

	done = false;

	do
	{
		MsgInfo("Which region in this city ? (0-3)\n");

		MsgInfo("Enter a number: ");
		scanf("%d", &settings.region);

		if (settings.region >= 0 && settings.region <= 3) done = true;
	} while (!done);

	done = false;

	do
	{
		MsgInfo("Which time ?\n");
		MsgInfo("\tDay: 0\n");
		MsgInfo("\tDawn: 1\n");
		MsgInfo("\tDusk: 2\n");
		MsgInfo("\tNight: 3\n");

		MsgInfo("Enter a number: ");
		scanf("%d", &settings.time);

		if (settings.time >= 0 && settings.time <= 3) done = true;
	} while (!done);

	done = false;
	
	do
	{
		int firstType = 0;
		int secondType = 0;
		
		MsgInfo("Which type of mission ?\n");
		MsgInfo("\tStart On Car: 0\n");
		MsgInfo("\tStart Data Required: 1\n");
		MsgInfo("\tStart On Foot: 2\n");

		MsgInfo("Enter the first type: ");
		scanf("%d", &firstType);
		
		MsgInfo("\tNone: -1\n");
		MsgInfo("\tCountdown: 3\n");
		MsgInfo("\tFelony Bar Not Active: 4\n");
		MsgInfo("\tFelony Bar No Fill: 5\n");

		MsgInfo("Enter the second type: ");
		scanf("%d", &secondType);

		settings.type = MSTYPE_ARR[firstType];
		if (secondType != -1) settings.type |= MSTYPE_ARR[secondType];
		done = true;
	} while (!done);

	done = false;

	if (!(settings.type & MSTYPE_StartOnFoot))
	{
		do
		{
			MsgInfo("Which car do you want to drive ?\n");
			MsgInfo("\tCops Car: 0\n");
			MsgInfo("\tUsual Cars: [1-4]\n");
			MsgInfo("\tSpecial Cars: [8-12]\n");
			MsgInfo("Enter car number: ");

			scanf("%d", &settings.playerCarModel);
			settings.playerCarColour = 0;

			MsgInfo("Enter max damage: ");

			scanf("%d", &settings.maxDamage);

			settings.residentModels[0] = settings.playerCarModel;
			if (settings.playerCarModel >= 0 && settings.playerCarModel <= 4 
				|| settings.playerCarModel >= 8 && settings.playerCarModel <= 12) done = true;
		} while (!done);

		done = false;

		do
		{
			MsgInfo("Which car color do you want ? (0-6)\n");

			scanf("%d", &settings.playerCarColour);

			if (settings.playerCarColour >= 0 && settings.playerCarColour <= 6) done = true;
		} while (!done);
	}

	done = false;

	MsgInfo("Resident Models\n");
	int j = settings.type & (settings.type & MSTYPE_StartOnFoot) ? 0 : 1;

	do
	{
		MsgInfo("ResidentModels[%d]: ", j);
		scanf("%d", &settings.residentModels[j]);
		j++;
		if (j == 5) done = true;
	} while (!done);

	done = false;
	
	MsgInfo("Enter the drowned message: ");
	scanf(" %64[^\n]", str);
	settings.msgDrowned = strlen(str) ? strings.addString(str) : -1;
	_flushall();
	
	MsgInfo("Enter the complete message: ");
	scanf(" %64[^\n]", str);
	settings.msgComplete = strlen(str) ? strings.addString(str) : -1;
	_flushall();
	
	MsgInfo("Enter the out of time message: ");
	scanf(" %64[^\n]", str);
	settings.msgOutOfTime = strlen(str) ? strings.addString(str) : -1;
	_flushall();
	
	MsgInfo("Enter the car wrecked message: ");
	scanf(" %64[^\n]", str);
	settings.msgCarWrecked = strlen(str) ? strings.addString(str) : -1;
	_flushall();
	
	MsgInfo("Enter the drowned message: ");
	scanf(" %64[^\n]", str);
	settings.msgDrowned = strlen(str) ? strings.addString(str) : -1;
	_flushall();

	MsgInfo("Enter the doors locked message: ");
	scanf(" %64[^\n]", str);
	settings.msgDoorsLocked = strlen(str) ? strings.addString(str) : -1;
	_flushall();
	
	done = false;
	
	do
	{
		MsgInfo("Enter X start position: ");
		scanf("%d", &settings.playerStartPosition.x);

		MsgInfo("Enter Y start position: ");
		scanf("%d", &settings.playerStartPosition.y);

		MsgInfo("Enter start rotation: ");
		scanf("%d", &settings.playerStartRotation);
		
		done = true;
	} while (!done);

	done = false;
	
	do
	{
		char answer;
		MsgInfo("Do you want a timer at the start ? (y / n) : ");
		scanf(" %c", &answer);
		if (answer == 'n') break;

		MsgInfo("How many seconds ? : ");
		scanf("%hd", &settings.timer);

		MsgInfo("What type of timer do you want ?\n");
		MsgInfo("\tActive: 0\n");
		MsgInfo("\tCounter: 1\n");
		MsgInfo("\tPaused: 2\n");
		MsgInfo("\tComplete on Out: 3\n");
		MsgInfo("\tBomb Countdown: 4\n");
		MsgInfo("\tBomb Triggered: 5\n");

		MsgInfo("Enter a number: ");
		
		scanf("%d", &value);

		if (value < 6 && value >= 0)
		{
			settings.timerFlags = TimerFlags_Arr[value];
			done = true;
		}
		
	} while (!done);

	done = false;
	
	do
	{
		MsgInfo("Enter minimum cops: ");
		scanf("%d", &settings.cops.min);

		MsgInfo("Enter maximum cops: ");
		scanf("%d", &settings.cops.max);

		MsgInfo("Enter power of the cops: ");
		scanf("%d", &settings.cops.power);

		MsgInfo("Enter speed of the cops: ");
		scanf("%d", &settings.cops.speed);

		done = true;
	} while (!done);

	done = false;

	MsgInfo("How many targets do you want ? :");
	scanf("%d", &nbTargets);
	
	int i = 0;
	do
	{
		MS_TARGET* target = &targets[i];

		printf("All targets: \n");
		for (int j = 0; j < nbTargets; j++)
		{
			printf("Target(%d)\n", j);
		}

		MsgInfo("New Target: \n");
		MsgInfo("Target(%d)", i);
		MsgInfo("\tPoint: 1\n");
		MsgInfo("\tCar: 2\n");
		// MsgInfo("\tEvent: 3\n");
		MsgInfo("\tPlayerToStart: 4\n");
		// MsgInfo("\tMultiCar: 5\n");
		MsgInfo("Finish: 5\n");

		MsgInfo("Which type do you want ? ");

		scanf("%d", &value);
		if (value == 3) break;
		target->type = value;

		if (target->type != Target_MultiCar)
		{
			MsgInfo("Common Target Flags\n");
			MsgInfo("\tFlag Active P1: 0\n");
			MsgInfo("\tFlag Active P2: 1\n");
			MsgInfo("\tFlag Completed P1: 2\n");
			MsgInfo("\tFlag Completed P2: 3\n");
			MsgInfo("\tFinish: 4\n");
			MsgInfo("Add target properties to finish enter 6\n");
			while (!done)
			{
				MsgInfo("Enter Target Flag (0-3) or 4 to finish: ");
				scanf("%d", &value);
				
				if (value == 4)
				{
					done = true;
					continue;
				}
				target->target_flags |= value;
			}

			done = false;
			
			MsgInfo("Common Display Flags\n");
			MsgInfo("\tFlag Visible P1: 4\n");
			MsgInfo("\tFlag Visible P2: 5\n");
			MsgInfo("\tFinish: 6\n");
			MsgInfo("Add target properties to finish enter 6\n");
			while (!done)
			{
				MsgInfo("Enter Target Flag (4-5) or 6 to finish: ");
				scanf("%d", &value);
				
				if (value == 6) 
				{
					done = true;
					continue;
				}
				target->target_flags |= TargetFlags[value];
			}

			done = false;
		}
		
		if (target->type == Target_Point)
		{
			MsgInfo("Pos X: "); scanf("%d", &target->point.posX);
			MsgInfo("Pos Z: "); scanf("%d", &target->point.posZ);
			MsgInfo("Radius: "); scanf("%d", &target->point.radius);
			MsgInfo("Pos Y: "); scanf("%d", &target->point.posY);
			MsgInfo("Height: "); scanf("%d", &target->point.height);
			MsgInfo("Lose Tail Message: "); scanf(" %64[^\n]", str);
			target->point.loseTailMessage = strlen(str) ? strings.addString(str) : -1;
			MsgInfo("Boat Offset X: "); scanf("%d", &target->point.boatOffsetX);
			MsgInfo("Boat Offset Z: "); scanf("%d", &target->point.boatOffsetZ);

			MsgInfo("Point Flags\n");
			MsgInfo("\tPoint CTF Base P1: 6\n");
			MsgInfo("\tPoint CTF Base P2: 7\n");
			MsgInfo("\tPoint CTF Flag: 8\n");
			MsgInfo("\tSecret Point 1: 9\n");
			MsgInfo("\tSecret Point 2: 10\n");
			MsgInfo("\tSecret Start Finish: 11\n");
			MsgInfo("\tOn Boat: 12\n");
			MsgInfo("\tStop Cops Trigger: 13\n");

			while (!done)
			{
				MsgInfo("Enter Point Flag (6-13) or 14 to finish: ");

				scanf("%d", &value);
				if (value == 14)
				{
					done = true;
					continue;
				}
				target->point.actionFlag |= TargetFlags[value];
			}

			done = false;
		}
		else if (target->type == Target_Car || target->type == Target_Player2Start)
		{
			MsgInfo("Pos X: "); scanf("%d", &target->car.posX);
			MsgInfo("Pos Z: "); scanf("%d", &target->car.posZ);
			MsgInfo("Pos Z: "); scanf("%d", &target->car.rotation);
			target->car.slot = -1;
			MsgInfo("Car Color: "); scanf("%d", &target->car.palette);
			MsgInfo("Type:\n");
			MsgInfo("\tChasing: 0\n");
			MsgInfo("\tTailing: 1\n");
			MsgInfo("Enter number: "); scanf("%d", &target->car.type);
			if (target->car.type != 3)
			{
				MsgInfo("Tail Close Message: "); scanf(" %64[^\n]", str);
				target->car.tail.closeMessages = strlen(str) ? strings.addString(str) : -1;

				MsgInfo("Tail Far Message: "); scanf(" %64[^\n]", str);
				target->car.tail.farMessages = strlen(str) ? strings.addString(str) : -1;
			}
			else
			{
				MsgInfo("Chase Too Far Message: "); scanf(" %64[^\n]", str);
				target->car.chasing.tooFarMessage = strlen(str) ? strings.addString(str) : -1;

				MsgInfo("Chase Getting Far Message: "); scanf(" %64[^\n]", str);
				target->car.chasing.gettingFarMessage = strlen(str) ? strings.addString(str) : -1;

				MsgInfo("Chase Max Damage: "); scanf("%d", &target->car.chasing.maxDamage);
			}
			
			MsgInfo("Cutscene: "); scanf("%d", &target->car.cutscene);
			MsgInfo("Max distance (-1 or more): "); scanf("%d", &target->car.maxDistance);

			MsgInfo("Car Flags\n");
			MsgInfo("\tCar Saved: 14\n");
			MsgInfo("\tPlayer Controlled: 15\n");
			MsgInfo("\tSwapped: 16\n");
			MsgInfo("\tPinged In: 17\n");

			while (!done)
			{
				MsgInfo("Enter action flag (14-17) or 18 to finish: ");

				scanf("%d", &value);
				if (value == 18)
				{
					done = true;
					continue;
				}
				target->car.flags |= TargetFlags[value];
			}
			
			done = false;
		}
		i++;
	} while (i < nbTargets);

	done = false;
	
	// Script System

	MsgInfo("How many threads do you want ?\n");
	scanf("%u", &nbThreads);

	Thread* threads = static_cast<Thread*>(malloc(sizeof(Thread) * nbThreads));

	while (i < nbThreads)
	{
		do
		{
			// Here ask which type of command they want to add
			for (int i = 0; i < 22; i++)
			{
				MsgInfo("%s: %d\n", COMMANDS_STR[i]);
			}
			MsgInfo("Finish Thread: 21\n");
			scanf("%d", &value);

			if (value >= 21 || value < 0) break;

			for (int j = 0; j < COMMANDS[value][1]; j++)
			{
				uint tmp_value = 0;
				MsgInfo("Enter argument (%d): ", j);
				scanf("%u", &tmp_value);
				push_thread(&threads[i], tmp_value);
			}
			push_thread(&threads[i], COMMANDS[value][0]);
			done = true;
		} while (!done);
		done = false;
		addThread(&stack, &threads[i]);
		i++;
	}
	
	processThreads(&stack);

	CompileMissionBlk(filename, settings, targets, stack, strings);
}

//-----------------------------------------------------

void PrintCommandLineArguments()
{
	MsgInfo("Example usage:\n");
	MsgInfo("\tDriver2MissionTool -make <mission_script_name.d2ms>\n");
	MsgInfo("\tDriver2MissionTool -decompile MISSIONS.BLK [mission_number]\n");
	MsgInfo("\tDriver2MissionTool -compile <mission_script_name.d2ms>\n");
}

int main(const int argc, char** argv)
{
#ifdef _WIN32
	Install_ConsoleSpewFunction();
#endif

	Msg("---------------\nDriverMissionTool - Driver 2 mission decompiler\n---------------\n\n");

	if (argc <= 1)
	{
		PrintCommandLineArguments();
		return -1;
	}

	for (int i = 0; i < argc; i++)
	{
		if (!stricmp(argv[i], "-make"))
		{
			if (i + 1 <= argc)
				CreateMission(argv[i + 1]);
			else
				MsgWarning("-make must have a filename argument!");

		}
	}

	return 0;
}