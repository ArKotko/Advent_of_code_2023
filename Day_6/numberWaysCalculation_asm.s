.file "numberWaysCalculation_asm.S"
.intel_syntax noprefix

.text
//raceTime --> rcx ; distanceRecord --> rdx

.global numberWaysCalculation_asm

numberWaysCalculation_asm:
	
		push    	rbp
        mov     	rbp, rsp
		//Calculates the maximum distance that can be travelled
		push 		rdx
		mov			rax, rcx //rax = raceTime
		xor 		rdx, rdx //rdx = 0
		mov 		r8, 2
		div 		r8 //(rax = raceTime / 2) & (rdx = raceTime % 2)
		mov 		r8, rcx //r8 = raceTime
		imul 		r8, r8 //r8 = raceTime ^ 2
		mov 		r9, r8 //r9 = raceTime ^ 2
		add 		r8, rdx //r8 = (raceTime ^ 2) - (raceTime % 2)
		shr			r8, 2 //r8 = ((raceTime ^ 2) - (raceTime % 2)) / 4 = maxRecord
		mov 		r11, rdx //Saves rdx (rdx = raceTime % 2)
		pop 		rdx
		//If the record equals this value, then the record can't be beaten
		xor 		rax, rax //rax = 0
		cmp 		rdx, r8
		jz			end	
		//Calculates the first iteration where the record is beaten
		shl			rdx, 2 //rdx = distanceRecord * 4
		sub 		r9, rdx //r9 = (raceTime ^ 2) - (4 * distanceRecord)
		cvtsi2sd 	xmm0, r9 //qword -> double
		sqrtpd  	xmm1, xmm0 //xmm1 = sqrt((raceTime ^ 2) - (4 * distanceRecord))
		cvtsd2si 	r9, xmm1 //double -> qword
		mov 		r10, rcx //r10 = raceTime
		sub 		r10, r9 //r10 = (raceTime) - sqrt((raceTime ^ 2) - (4 * distanceRecord))
		shr			r10, 1 //r10 = ((raceTime) - sqrt((raceTime ^ 2) - (4 * distanceRecord))) / 2
		inc 		r10 //r10 = it_higherDistance
		mov 		rax, rcx //rax = raceTime
		shr 		rax, 1 //rax = raceTime / 2 = it_maxDistance
		sub 		rax, r10 //rax = it_maxDistance - it_higherDistance
		inc 		rax //rax = (it_maxDistance - it_higherDistance) + 1
		shl			rax, 1 //rax = ((it_maxDistance - it_higherDistance) + 1) * 2
		cmp 		r11, 0 //Checks if raceTime is an odd or an even number
		jnz			end
		dec 		rax //If it is an even number, subtracts one to the result
		
end:
		pop 		rbp
		ret