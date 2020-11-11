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

    /*ofstream myfile;
    myfile.open("markets.txt", std::ios_base::app);*/

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


    vector<Bidder> bidders(num_bidders);

    for (int k = 0; k < num_bidders; ++k) {
        bidders[k].valuation.resize(num_goods);
        //valuation pro Gut und Bidder
        for (auto &v: bidders[k].valuation) v = (random_number(0, 11) + random_number(0, 15)) * i;
        bidders[k].budget = random_number(0, 11) + random_number(0, 31);
        bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
    }

    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;

   //int num_iterations = 2000;
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


    //ich benötige die utility zu Beginn des Programms, um diese am Ende für die gerundeten Kanten mal 1 zu nehmen
    // und mit der max_utility aus dem fraktionalen Teil zu Vergleichen

    for (int i = 0; i < num_bidders; ++i) {
        cout << utility[i] << endl;
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
        cout << "Demand: " << demand << endl;
        //cout << "Supply: " << prices[j] << endl;
    }

    //set precision
    int pre = 3;


    //macht das Sinn? Summe der Max_utils?
    double max_util = 0;

    for (int i = 0; i < num_bidders; ++i) {
        cout << "Max Utility: " << max_utility[i] << std::setprecision(pre) << endl;
        max_util = max_util + max_utility[i];
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

    /*** Write allocations to graph ***/
    vector<vector<double>> graph(num_bidders, vector<double>(num_goods));
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            graph[i][j] = bidders[i].spent[j] / prices[j];
        }
    }



    /*** hier können 2 bidder das gleiche Gut bekommen ***//*

    cout << "\n";
    for (int j = 0; j < num_goods; ++j) {
        for (int i = 0; i < num_bidders; ++i) {
            cout << 100*(graph[i][j]) << " ";
        }
    }*/


    /*** wir tauschen hier bidder und goods, sodass nicht 2 Bidder das gleiche Gut zugewiesen bekommen ***/

    /* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */

    /*multiplizieren die Allokation mit 20, da wir einen Wert > 1 erhalten wollen, um iterative Rounding
     * nutzen zu können */
/*
    cout << "\n";
    cout << "\n";
    cout << "Allokation/Kantengewicht: \n";
    *//*** print graph ***//*
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            if ((20 * (graph[i][j])) < 0.001) {
                graph[i][j] = 0;
            }
            cout << 20 * (graph[i][j]) << " ";
        }
        cout << " | ";
    }
    cout << "\n";*/

    /*** runde ich die Nachkommastellen, während ich die Vorkommastelle behalte; ***/


    //fractional and integral parts


    /*ofstream myfile2;
    myfile2.open("test.txt", std::ios_base::app);*/



    //for-schleifen hier getauscht, da wir pro Gut (über alle Bidder) die kummulierte Anzahl wollen//

    //frac enthält die fraktionalen Teile der Allokation

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




 /*   cout << "\n";
    cout << "Valuation der Güter (jeweils für einen Bidder Gut 1 bis " << num_goods << "): \n ";
    for (int b = 0; b < num_bidders; ++b) {
        for (int i = 0; i < num_goods; ++i) {
            cout << bidders[b].valuation[i] << " ";

        }
        cout << " | ";
    }*/

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



    /*cout << "\n";
    cout << "integral Allokation/Kantengewicht: \n";
    *//*** print graph ***//*
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            cout << std::setprecision(pre) << floor(20 * (graph[i][j])) << " ";
        }
        cout << " | ";
    }
    cout << "\n";*/

   /* cout << "\n";
    cout << "fractional Allokation/Kantengewicht: \n";
    *//*** print graph ***//*
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            if ((20 * (graph[i][j])) < 0.001) {
                graph[i][j] = 0;
            }

            cout << std::setprecision(pre) << (20 * (graph[i][j]) - floor(20 * (graph[i][j]))) << " ";

        }
        cout << " | ";
    }
    cout << "\n";*/

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


    /*cout << " \n";
    cout << "Money spend for good (jeweils für einen Bidder Gut 1 bis " << num_goods << "): \n";
    for (int b = 0; b < num_bidders; ++b) {
        for (int i = 0; i < num_goods; ++i) {
            if (bidders[b].spent[i] < (10 ^ -10)) {
                bidders[b].spent[i] = 0;
            }
            cout << std::fixed << (bidders[b].spent[i]) << " ";
        }
        cout << " | ";

    }*/

    //Budget der Bidder
/*
    cout << " \n ";
    cout << "Budget der Bidder: ";
    for(int b=0; b< num_bidders; ++b) {
        cout << (bidders[b].budget)<< " ";
    }
*/

    /*cout << " \n";
    cout << "Preise der Güter: ";
    for (int i = 0; i < num_goods; ++i) {
        cout << prices[i] << " ";
    }*/


    //graph[][] := anfängliche/fraktionale Allokationen


   /* alte (nicht gerundete max_util pro Bidder:
    double util = 0.0;
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            util = util + ((bidders[i].spent[j])/(prices[j])) * bidders[i].valuation[j];
        }
        cout << rd_util << "| ";
        util = 0.0;
    }*/


    //max_utility der gerundeten Alloks berechnen
    cout << "\n";
    cout << "\n";
    cout << "\n";
    cout << "max_utility (for rounded alloc): \n";
    double rd_util = 0.0;
    vector<double> rd_max_utility(num_bidders);
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            rd_util = rd_util + (((up_integral[i][j]) / 20.0) * bidders[i].valuation[j]);
        }
        rd_max_utility[i] = rd_util;
        cout << rd_util << " | ";
        cout << max_utility[i] << " | ";
        if(rd_max_utility[i] > max_utility[i]){
            cout << 1- (max_utility[i]/rd_max_utility[i]) << " % weniger  \n";
        }
        else{
            cout << 1-(rd_max_utility[i]/max_utility[i]) << " % weniger \n";
        }
        //cout << " \n";
        rd_util = 0.0;
    }

   /* cout << "\n";
    cout << "Differenz Allokationen (optimal/gerundet): \n";
    double diff = 0.0;
    for (int i = 0; i < num_bidders; i++) {
        for (int j = 0; j < num_goods; ++j) {
            // achtung: graph-value * 20
            diff = up_integral[i][j]-(20 * graph[i][j]); //ohne abs
            cout << diff << "  ";
        }
        cout << " | ";
    }*/



    //vergleich optimal und gerundet

    //max_utility[i] = optimal
    //rd_max_utility[i] = gerundet

    ofstream myfile;
    myfile.open ("markets.txt", std::ios_base::app);
    cout << "\n";
    myfile << "Number Goods: " << num_goods << " Number Bidders: " << num_bidders << " Number Iterations: " << num_iterations << "\n";
    cout << "\n";
    myfile << "Maximierter Nutzen bei fraktionale Allokation" << " , " << "Maximierter Nutzen bei integraler Allokation: " << "\n";
    for (int i = 0; i < num_bidders; i++) {
        cout << "\n Nutzen des Bidders " << i << " hat sich um " << abs(((max_utility[i]) - (rd_max_utility[i]))) << " verändert.\n";
        myfile << "Bidder " << i << ": "<< (max_utility[i]) << " , " << (rd_max_utility[i]) << endl;
    }
    myfile << "Highest Valuation for Good 1 to " << num_goods << " and the bidder with this specific valuation: " << "\n";
    for (int i = 0; i < num_goods; ++i) {
        for (int b = 0; b < num_bidders; ++b) {
            if (bidders[b].valuation[i] >= greatest_val) {
                greatest_val = bidders[b].valuation[i];
                vecPair[i] = make_pair(greatest_val, b);
            }
        }
        myfile << "(" << vecPair[i].first << "," << vecPair[i].second << ")";
        myfile << " | ";
        myfile << endl;
        greatest_val = 0;
    }
    myfile << endl;

    //maximales element des vectors
    double money = 0.0;
    vector<pair<int, double> > vecPairBudget(1);
    for (int b = 0; b < num_bidders; ++b) {
        if(bidders[b].budget > money) {
            money = bidders[b].budget;
            vecPairBudget[1].first = b;
            vecPairBudget[1].second = money;
        }
    }
    myfile << "Biggest Budget: " << money << " is available by Bidder " <<  vecPairBudget[1].first << "\n";
    myfile << endl;
    myfile.close();






    return 0;

}






/* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */
/*TODO:
    - Neue MaxUtility berechnen mit integralem Anteil und dem entsprechen per Entscheidungsverfahren
        zugeteilten fraktionalen Anteil
    */


