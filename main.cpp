#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <chrono>
#include <random>
#include <list>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <ctime>

//Proportional Response Dynamics

// returned:
// - Equilibrium prices
// - equilibrium allocations

//Paper: Distributed Algorithms via Gradient Descent for Fisher Markets

//floor function eingefügt


// gibt es pro Gut genau eine Einheit??? -> Annahme ja! (22.10.20)

using namespace std;

class Bidder {
public:
    vector<double> valuation; //was mir ein gut wert ist
    double budget;
    vector<double> spent; //für welches gut gibt bidder was aus (summe aller elem in spent ist budget)

    friend ostream &operator<<(ostream &os, const Bidder &b);
};

ostream &operator<<(ostream &os, const Bidder &b) {
    for (int j = 0; j < b.spent.size(); ++j) {
        os << b.spent[j] << " ";
    }
    return os;
}

int random_number(int lb, int ub) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 engine(seed);
    return (engine() % (ub - lb + 1)) + lb;
}

//Main method
int main() {


    auto start = std::chrono::system_clock::now();

    /*
   *
   *
   * HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
   *
   *
   */

    //generate #goods
    int num_goods;
    cout << "Number Goods: ";
    cin >> num_goods;
    //int num_goods = 6;


    //vector<Bidder> bidders(5);

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders;
    cout << "Number Bidders: ";
    cin >> num_bidders;
    //int num_bidders = 10;

    if (num_bidders > num_goods) {
        printf("Error number bidders larger than number goods");
        exit(EXIT_FAILURE);
    }

    //num_iterations = Anzahl der Iterationen des Handels auf dem FM
    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;

    //Name der file, in die geschrieben wird
    string filename;
    cout << "Welchen Namen soll die File haben? ";
    cin >> filename;


    //num_iter_exp = Anzahl Ausführungen des Gesamtexperiments
    int num_iter_exp;
    cout << "Number Iterations Experiment: ";
    cin >> num_iter_exp;

    //FOR SCHLEIFE FÜR ANZAHL WIEDERHOLUNGEN DES GESAMTEXPERIMENTS
    for (int iter = 0; iter < num_iter_exp; iter++) {


        /*
        *
        * BIS HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
        *
        *
        *
        */


        vector<Bidder> bidders(num_bidders);

        for (int k = 0; k < num_bidders; ++k) {
            bidders[k].valuation.resize(num_goods);
            //valuation pro Gut und Bidder
            for (auto &v: bidders[k].valuation) v = (random_number(0, 11) + random_number(0, 15)) * i;
            bidders[k].budget = random_number(0, 11) + random_number(0, 31);
            bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
        }


        vector<double> prices(num_goods);
        for (int it = 0; it < num_iterations; ++it) {

            //in jeder iteration werden die preise des guts i auf die menge der preise,
            // die jeder bidder ausgegeben hat, gesetzt
            for (int j = 0; j < num_goods; ++j) {
                prices[j] = 0;
                for (int i = 0; i < bidders.size(); ++i)
                    prices[j] += bidders[i].spent[j];

            }
            //update der valuations und spents pro bidder
            vector<vector<double>> update(bidders.size(), vector<double>(num_goods)); //
            for (int i = 0; i < bidders.size(); ++i) {
                for (int j = 0; j < num_goods; ++j) {
                    update[i][j] = bidders[i].valuation[j] * bidders[i].spent[j] / prices[j];

                }
            }

            //new bid vector for next iteration
            for (int i = 0; i < bidders.size(); ++i) {
                for (int j = 0; j < num_goods; ++j) {
                    bidders[i].spent[j] =
                            bidders[i].budget * update[i][j] / accumulate(update[i].begin(), update[i].end(), 0.0);

                }
            }

            //print für jeden bidder und jede iteration dessen ???
            cout << "Iteration " << it << ":\n";
            for (int i = 0; i < bidders.size(); ++i) {
                cout << "Bidder " << i << ": " << bidders[i] << endl;
            }
            cout << endl;

            //writing to txt file

            /*ofstream myfile;
            myfile.open ("markets.txt", std::ios_base::app);
            myfile << "Iteration " << it << ":\n";
            for (int i = 0; i < bidders.size(); ++i) {
                myfile << "Bidder " << i << ": " << bidders[i] << endl;
            }
            myfile << endl;*/
        }

        //von Max utility und utility (im equilibrium sind diese gleich)

        vector<double> utility(num_bidders);
        vector<double> max_utility(num_bidders);
        for (int b = 0; b < num_bidders; ++b) {
            max_utility[b] = 0;
            for (int i = 0; i < num_goods; ++i) {
                utility[b] += bidders[b].valuation[i] * bidders[b].spent[i] / prices[i]; //Aufpassen wenn prices[i] = 0!
                if (max_utility[b] < bidders[b].valuation[i] / prices[i]) {
                    max_utility[b] = bidders[b].valuation[i] / prices[i];
                }
            }

            max_utility[b] *= bidders[b].budget;
        }

        // save utility from start
        vector<double> val_start(num_bidders);
        for (int b = 0; b < num_bidders; ++b) {
            for (int i = 0; i < num_goods; ++i) {
                val_start[b] = bidders[b].valuation[i];
            }
        }



        //Optimales Ergebnis//

        cout << endl;
        cout << "Fraktionales/optimales Ergebnis: ";
        cout << endl;
        for (int j = 0; j < num_goods; ++j) {
            double demand = 0;
            double supply = 0;
            for (int i = 0; i < bidders.size(); ++i) {
                demand += bidders[i].spent[j] / prices[j];
            }
            //cout << "Demand: " << demand << endl;
            //cout << "Supply: " << prices[j] << endl;
        }

        //set precision
        int pre = 3;


        //macht das Sinn? Summe der Max_utils?
        //double max_util = 0;

        for (int i = 0; i < num_bidders; ++i) {
            cout << "Max Utility: " << max_utility[i] << std::setprecision(pre) << endl;
            //max_util = max_util + max_utility[i];
        }

        //cout << "Summe Max Utility: " << max_util;



        //                      //
        //                      //
        //                      //
        // writing to txt file  //
        //                      //
        //                      //
        //                      //

        /*  *//*ofstream myfile;
    myfile.open ("markets.txt", std::ios_base::app);*//*
    myfile << endl;
    myfile << "D/MaxUtil: " << endl;
    for (int j = 0; j < num_goods; ++j) {
        double demand = 0;
        double supply = 0;
        for (int i = 0; i < bidders.size(); ++i) {
            demand += bidders[i].spent[j] / prices[j];
            myfile << "Bidder " << i << " receives " << bidders[i].spent[j] / prices[j] << "of good " << j << endl;
        }
        myfile << "Demand: " << demand << endl;
        myfile << "\n";
    }
     myfile.close();


*/


/* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */
        /*** Write allocations to graph ***/
        vector<vector<double>> graph(num_bidders, vector<double>(num_goods));
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                graph[i][j] = bidders[i].spent[j] / prices[j];
            }
        }


        double frac = 0;


        //cout << "summe fractional Gut 1 bis " << num_goods << ": \n";
        vector<int> fracVec(num_goods);
        for (int j = 0; j < num_goods; ++j) {
            for (int i = 0; i < num_bidders; ++i) {
                if ((20 * (graph[i][j])) < 0.001) {
                    graph[i][j] = 0;
                }
                frac = frac + (20 * (graph[i][j]) - floor(20 * (graph[i][j])));
            }

            /*fracVec[j] = round(frac);
            cout << std::setprecision(pre) << frac << " ";
            cout << " | ";
            if (j == (num_goods - 1)) {
                myfile2 << std::setprecision(pre) << frac;
                continue;
            }
            myfile2 << std::setprecision(pre) << frac << " , ";
            frac = 0;*/
        }
        /* myfile2 << endl;
         myfile2.close();*/



        //sortiere höchste valuation für jeweiliges gut raus
        cout << "\n";
        cout << "Höchste Valuation pro Gut (Achtung: Bidder werden ab 0 gezählt) \n ";

        vector<pair<int, int> > vecPair(num_goods);

        //höchste Valuation für jeweils ein Gut = Entscheidung wem die fraktionalen Teilen eines Guts
        // zugewwiesen werden

        int greatest_val = 0;
        for (int i = 0; i < num_goods; ++i) {
            for (int b = 0; b < num_bidders; ++b) {
                if (bidders[b].valuation[i] >= greatest_val) {
                    greatest_val = bidders[b].valuation[i];
                    vecPair[i] = make_pair(greatest_val, b);
                }

            }
            cout << "(" << vecPair[i].first << "," << vecPair[i].second << ")";
            cout << " | ";
            greatest_val = 0;
        }
        cout << endl;

        vector<vector<int>> up_integral(num_bidders, vector<int>(num_goods));
        //Initialisiere mit integraler Allokation
        /*up_integral := die integralen Ergebnisse, nachdem die fraktionalen Teile den jeweiligen Biddern
        zugeteilt wurden */
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                up_integral[i][j] = floor(20 * (graph[i][j]));
            }
        }
        for (int j = 0; j < num_goods; ++j) {
            up_integral[vecPair[j].second][j] += fracVec[j];
        }


        cout << "\n";
        cout << "\n";
        cout << "Optimale Allokation: \n";
        /*** print graph ***/
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                if ((20 * (graph[i][j])) < 0.001) {
                    graph[i][j] = 0;
                }
                cout << 20 * (graph[i][j]) << " ";
            }
            cout << " | ";
        }
        cout << "\n";

        //neue Allokation nach dem Runden der Allokationen:
        cout << "\n";
        cout << "Update integrale (gerundete) Allokation: \n";
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << up_integral[i][j] << " ";
            }
            cout << " | ";
        }
        cout << endl;



        //max_utility der gerundeten Alloks berechnen
        cout << "\n";
        cout << "\n";
        cout << "\n";
        cout << "max_utility for rounded alloc: | max_utility: | integrality gap: \n";
        double rd_util = 0.0;
        vector<double> rd_max_utility(num_bidders);

        double int_gap = 0.0;
        double print_int_gap = 0.0;
        double avg_int_gap = 0.0;


        ofstream myfile2;
        myfile2.open(filename + "_table" + ".txt", std::ios_base::app);

        myfile2 << "Number Goods: " << num_goods << ", " << " Number Bidders: " << num_bidders << ", "  << " Number Iterations: " << num_iterations << "\n";

        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                rd_util = rd_util + (((up_integral[i][j]) / 20.0) * bidders[i].valuation[j]);
            }
            rd_max_utility[i] = rd_util;
            myfile2 << rd_max_utility[i] << " | ";
            myfile2 << std::setprecision(pre)  << max_utility[i] << "\n";

            if(rd_max_utility[i] <= max_utility[i]){
                cout << std::setprecision(pre)  << rd_max_utility[i]/max_utility[i] << "\n";
                //myfile << std::setprecision(pre)  << rd_max_utility[i]/max_utility[i] << "\n";
                int_gap = int_gap + (rd_max_utility[i]/max_utility[i]);
            }
            if(rd_max_utility[i] > max_utility[i]){
                cout << std::setprecision(pre)  << max_utility[i]/rd_max_utility[i] << "\n";
                //myfile << std::setprecision(pre)  << max_utility[i]/rd_max_utility[i] << "\n";
                int_gap = int_gap + (max_utility[i]/rd_max_utility[i]);
            }

            print_int_gap = int_gap;
            if(i==(num_bidders-1)){
                avg_int_gap = print_int_gap; // /num_bidders;
            }
            int_gap = 0.0;
            rd_util = 0.0;
        }

        myfile2 << "Integrality gap is in average: " << avg_int_gap;
        myfile2 << "\n";
        myfile2 << "\n";
        myfile2 << "\n";




        //vergleich optimal und gerundet

        //max_utility[i] = optimal
        //rd_max_utility[i] = gerundet

        /*ofstream myfile;
        myfile.open("markets.txt", std::ios_base::app);
*/







    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    if(iter == (num_iter_exp-1)){
        myfile2 << "finished computation at " << std::ctime(&end_time)
                << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }

    }

    return 0;

}


