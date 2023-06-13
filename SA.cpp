void NPE::SimulatedAnnealing(vector<string>& s){

	const int N=nmoves*int(m.size());// calculates total number of uphill moves

	vector <string> best=s; // best stores best optimized topology

	vector <string> temp=s; // temp stores temporary optimized topology

	float T0=initialTemp(temp); // calculates initial temperature based on temporary topology for immediate iteration

	int totm, uphill, reject; // total moves, uphill moves, rejected moves respectively

	float T=T0; // initial temperature assigned to T

	int i=0;

	do{

		totm=0; uphill=0; reject=0;

		do{

			float oc=do_cost(temp); // cost of initial topology assigned to oc

			int a=rand()%3+1; // randomly chooses a number between 1 to 3 for M move

            switch(a) {

                case 1: {

                    M1(temp);// M1 move applied if randomly chosen number is 1

                    break;

                }

                case 2: {

                    M2(temp);// M2 move applied if randomly chosen number is 2

                    break;


                }
                case 3: {


                    M3(temp);// M3 move applied if randomly chosen number is 3

                    break;

                } 
		    }

            totm++; // increased the number of total moves by 1

            float nc=do_cost(temp);//calculates new cost of topology after an M move

            float d=nc-oc; // calculates difference of old cost and new cost

            double r=((double)rand()/(RAND_MAX)); // randomly picks a number between 0 and 1 and assigns it to r

            if(d<0||(r<exp(-1*d/T))) { // if new cost is less than old cost or if r is less than exp(-d/T)


                if(d>0) { // if new cost is more than old cost, allows uphill move

                    uphill++; // increments the number of uphill moves by 1


                }

                s=temp; // accept the new topology

                if(do_cost(s)<do_cost(best)) { // checks if cost of new topology is less than cost of best

                    best=s; // new topology assigned as best

                }

            } else {

                reject++; // increments number of rejected moves by 1

            }

        } while(uphill < N && totm <= 2 * N); // checks if number of uphill moves is less than N and total number of moves less than 2*N

        T=T<lambdatf*T0?0.1*T:ratio * T; // schedules new temperature

        i++;

    } while (reject/totm <= 0.95 && T >= epsilon); // checks if rejected moves is less than 95% of total moves and T is not less than epsilon



    s=best; //accepts new optimized topology


}
