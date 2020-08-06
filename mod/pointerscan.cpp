#include "pointerscan.hpp"

namespace mod
{

	void cPointerScanner::rscan(void* valuetofind, signed level)
	{

		// var

		unsigned char* p;
		__int32* pd;
		__int64* pq;

		signed i, j;
		bool addedToQueue;

		bool ExactOffset;

		ptruint startvalue;
		ptruint stopvalue;
		//ppointerlist plist;

		//tstaticdata nostatic;
		bool DontGoDeeper;
		int DifferentOffsetsInThisNode;
		bool locked;

		{//begin /* 639 */
			//if (level >= maxlevel) or (terminated and (savestate = false)) then
			if (level >= maxlevel || terminated && (savestate == false)) {
				//exit;
				return;
			}

			//currentlevel:=level;
			currentlevel = level;
			//DifferentOffsetsInThisNode: = 0;
			DifferentOffsetsInThisNode = 0;

			//exactOffset:=mustEndWithSpecificOffset and (length(mustendwithoffsetlist)-1>=level);
			ExactOffset = mustEndWithSpecificOffset && (length(mustendwithoffsetlist) - 1 >= level);

			//if exactOffset then
			if (ExactOffset)
			{//begin /* 651 */
				//startvalue:=valuetofind-mustendwithoffsetlist[level];
				//stopvalue: = startvalue;
				startvalue = valuetofind - mustendwithoffsetlist[level];
				stopvalue = startvalue;
			}//end /* 654 */
			else /* 655 */
			{//begin /* 656 */
				//startvalue:=valuetofind-structsize;
				startvalue = valuetofind - structsize;
				//stopvalue:=valuetofind;
				stopvalue = (ptruint)valuetofind;
				//if NegativeOffsets then inc(stopvalue, structsize);
				if (NegativeOffsets) inc(stopvalue, structsize);

				//if startvalue>stopvalue then startvalue:=0;
				if (startvalue > stopvalue) startvalue = 0;
			}//end /* 678 */

			//if noLoop then
			if (noLoop)
			{//begin /* 682 */
				/* check if this valuetofind is already in the list */
				//for i:=0 to level-1 do
				for (auto i = 0; i <= level - 1; i++) {
					//if valuelist[i]=valuetofind then
					if ( valuelist[i]=valuetofind )
					{//begin /* 686 */
						return;//exit;
					}//end /* 688 */
				}

				/* add this valuetofind to the list */
				//valuelist[level]: = valuetofind;
				valuelist[level] = valuetofind;
			}//end /* 692 */

			/* lastaddress:=maxaddress; */

			//LookingForMin:=startvalue;
			LookingForMin = startvalue;
			//LookingForMax:=stopvalue;
			LookingForMax = stopvalue;

			//dontGoDeeper:=false;
			DontGoDeeper = false;
			//plist:=nil;
			plist = __nullptr;
			//while stopvalue>=startvalue do
			while (stopvalue >= startvalue)
			{//begin /* 702 */
				//if plist=nil then
				if (plist == __nullptr) {
					//plist:=pointerlisthandler.findPointerValue(startvalue, stopvalue);
					plist = pointerlisthandler.findPointerValue(startvalue, stopvalue);
				}

				//if plist<>nil then
				if (plist != __nullptr)
				{//begin /* 707 */
					/*{ if stopvalue > valuetofind then
						asm
						nop
						end;  }*/
					//tempresults[level]:=valuetofind-stopvalue; /* store the offset */
					tempresults[level] = valuetofind - stopvalue;

					/* go through the list of addresses that have this address(stopvalue) as their value */
					//for j: = 0 to plist.pos - 1 do
					for (auto j = 0; j <= plist.pos - 1; j++)
					{//begin /* 719 */
						/*	{$ifdef benchmarkps}
							inc(pathsevaluated);
							{$endif}	*/

						//if (plist.list[j].staticdata=nil) then /* this removes a lot of other possible paths. Perhaps a feature to remove this check ? */
						if (plist.list[j].staticdata==__nullptr)
						{//begin /* 726 */
							//if (not dontGoDeeper) then
							if (!DontGoDeeper)
							{//begin /* 728 */
								/* check if we should go deeper into these results (not if max level has been reached) */

								//if (level+1) < maxlevel then
								if ( (level+1) < maxlevel )
								{//begin /* 733 */
									//addedToQueue:=false;
									addedToQueue = false;
									
									/* if there is not enough diskspace left wait till it's terminated, or diskspace is freed */
									/*
									//if (not terminated) and (not outofdiskspace^) then
									//{//begin
									//		if (
									//			(level + 3 < maxlevel) and
									//			(
									//				((pathqueuelength ^ < MAXQUEUESIZE - (MAXQUEUESIZE div 3))) or
									//				((level <= 2) and (pathqueuelength ^ < MAXQUEUESIZE - (MAXQUEUESIZE div 8))) or
									//				((level <= 1) and (pathqueuelength ^ < MAXQUEUESIZE - (MAXQUEUESIZE div 16))) or
									//				((level = 0) and (pathqueuelength ^ < MAXQUEUESIZE - 1))
									//				)
									//			)
									//			or
									//			(pathqueuelength ^= 0) //completely empty
									//			then //there's room and not a crappy work item. Add it
									//			begin
									//			if (not Terminated) or savestate then
									//				begin
									//				//try to lock multiple times if high level pointers
									//				locked : = pathqueueCS.tryEnter;
									//	if not locked and (level <= 2) then locked : = pathqueueCS.tryEnter;
									//	if not locked and (level <= 1) then
									//		begin
									//		//Two previous locks failed. Yield and try a lock again
									//		sleep(0);
									//locked: = pathqueueCS.tryEnter;
									//	if not locked then
									//		begin
									//		//one more time
									//		sleep(0);
									//locked: = pathqueueCS.tryEnter;
									//	end;
									//	end;

									//	if not locked and (level = 0) then
									//		begin
									//		//I must have this lock
									//		pathqueueCS.Enter;
									//locked: = true;
									//	end;


									//	if locked then
									//		begin
									//		if pathqueuelength^ < MAXQUEUESIZE - 1 then
									//			begin
									//			//still room

									//			CopyMemory(@pathqueue[pathqueuelength^].tempresults[0], @tempresults[0], maxlevel * sizeof(dword));
									//	if noLoop then
									//		CopyMemory(@pathqueue[pathqueuelength^].valuelist[0], @valuelist[0], maxlevel * sizeof(ptruint));

									//	pathqueue[pathqueuelength^].startlevel: = level + 1;
									//	pathqueue[pathqueuelength^].valuetofind: = plist.list[j].address;

									//	inc(pathqueuelength^);
									//	{$ifdef windows}
									//	ReleaseSemaphore(pathqueueSemaphore, 1, nil);
									//	{$else}
									//	pathqueueSemaphore.Release;
									//	{$endif}

									//addedToQueue: = true;
									//	end;
									//	pathqueueCS.Leave;
									//	end;
									//	end
									//	else
									//		exit;
									//	end;


									//	if not addedToQueue then
									//		begin
									//		//I'll have to do it myself
									//		rscan(plist.list[j].address, level + 1);

									//	///done with this branch
									//	end;

									//	end
									//	else
									//		begin

									//		//!!Out of diskspace or terminated!!

									//		if outofdiskspace^ or savestate then //save to the overflowqueueu and return
									//			begin
									//			//fill in the temppathqueue and send it to the overflowqueuewriter
									//			temppathqueue.startlevel: = level + 1;
									//	temppathqueue.valuetofind: = plist.list[j].address;

									//	if length(temppathqueue.tempresults) < length(tempresults) then
									//		setlength(temppathqueue.tempresults, length(tempresults));

									//	CopyMemory(@temppathqueue.tempresults[0], @tempresults[0], maxlevel * sizeof(dword));

									//	if length(temppathqueue.valuelist) < length(valuelist) then
									//		setlength(temppathqueue.valuelist, length(valuelist));

									//	CopyMemory(@temppathqueue.valuelist[0], @valuelist[0], maxlevel * sizeof(PtrUInt));

									//	overflowqueuewriter(self, temppathqueue);
									//	end
									//	else
									//		exit;//it was a terminate

									//	  //^^^^out of diskspace or save state!^^^^

									//	
									//}end;
									*/

									//if (not staticonly) then //store this results entry
									if (!staticonly)
									{//begin /* 848 */
										//nostatic.moduleindex: = $FFFFFFFF;
										nostatic.moduleindex = 0xFFFFFFFF;
										//nostatic.offset: = plist.list[j].address;
										nostatic.offset = plist.list[j].address;
										//StorePath(level, -1, plist.list[j].address);
										StorePath(level, -1, plist.list[j].address);
									}//end; /* 852 */
								}//end /* 854 */
								else /* 855 */
								{//begin /* 856 */
									/* end of the line */
									//if (not staticonly) then //store this results entry
									if (!staticonly)
									{//begin /* 859 */
										//nostatic.moduleindex: = $FFFFFFFF;
										nostatic.moduleindex = 0xFFFFFFFF;
										//nostatic.offset: = plist.list[j].address;
										nostatic.offset = plist.list[j].address;
										//StorePath(level, -1, plist.list[j].address);
										StorePath(level, -1, plist.list[j].address);
									}//end; /* 863 */
								}//end /* 864 */
							}//end /* else don't go deeper */ /* 866 */
						}//end /* 867 */
						else /* 868 */
						{//begin /* 869 */
							/* found a static one */
							//StorePath(level, plist.list[j].staticdata.moduleindex, plist.list[j].staticdata.offset);
							StorePath(level, plist.list[j].staticdata.moduleindex, plist.list[j].staticdata.offset);
							//if onlyOneStaticInPath then DontGoDeeper:=true;
							if (onlyOneStaticInPath) DontGoDeeper = true;
						}//end /* 874 */
					}//end /* 875 */

					/* check if the current itteration is less than maxOffsetsPerNode */
					//if LimitToMaxOffsetsPerNode then
					if (LimitToMaxOffsetsPerNode)
					{//begin /* 879 */
						//if level>0 then
						if (level > 0) {
							//inc(DifferentOffsetsInThisNode);
							DifferentOffsetsInThisNode++;
						}

						//if (DifferentOffsetsInThisNode>=maxOffsetsPerNode) then
						if (DifferentOffsetsInThisNode >= maxOffsetsPerNode) {
							/* the max node has been reached */
							//exit
							return;
						}
					}//end /* 885 */

					//plist:=plist.previous;
					plist = plist.previous;
					//if plist<>nil then
					if (plist != __nullptr) {
						//stopvalue:=plist.pointervalue
						stopvalue = plist.pointervalue;
					}
					else /* 891 */
					{
						/* nothing else to be found */
						//exit
						return;
					}
				}//end /* 894 */
				else /* 894 */
				{//begin /* 895 */
					//exit
					return;
				}//end /* 900 */
			};//end /* 902 */
		}//end /* 903 */

	}

}