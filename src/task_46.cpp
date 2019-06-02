#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include "ArgParseStandalone.h"
#include "utilities.h"

class bot {
    public:
        typedef long type;
        bot() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->r = 0;
        }
        bot(type x, type y, type z, type r) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->r = r;
            this->num_int = 0;
        }
        bot(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
            this->num_int = rhs.num_int;
        }
        bot& operator=(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
            this->num_int = rhs.num_int;
            return (*this);
        }
        bot cross(const bot& rhs) const {
            return bot(this->y*rhs.z-this->z*rhs.y,
                       this->z*rhs.x-this->x*rhs.z,
                       this->x*rhs.y-this->y*rhs.x, 0);
        }
        bot::type dot(const bot& rhs) const {
            return this->x*rhs.x+this->y*rhs.y+this->z*rhs.z;
        }
        double mag() {
            return std::sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
        }
        bool operator==(const bot& rhs) const {
            if((this->x != rhs.x)||(this->y != rhs.y)||
               (this->z != rhs.z)||(this->r != rhs.r)) {
                return false;
            } else {
                return true;
            }
        }
        bool operator!=(const bot& rhs) const {
            return !((*this) == rhs);
        }
        int dist(const bot& rhs) const {
            int answer = 0;
            answer += std::abs(this->x-rhs.x);
            answer += std::abs(this->y-rhs.y);
            answer += std::abs(this->z-rhs.z);
            return answer;
        }
        bot operator+(const bot& rhs) const {
            return bot(this->x+rhs.x, this->y+rhs.y, this->z+rhs.z, this->r+rhs.r);
        }
        bot operator-(const bot& rhs) const {
            return bot(this->x-rhs.x, this->y-rhs.y, this->z-rhs.z, this->r-rhs.r);
        }
        bot operator*(double a) const {
            return bot(this->x*a, this->y*a, this->z*a, this->r*a);
        }
        bot operator/(double a) const {
            return bot(this->x/a, this->y/a, this->z/a, this->r/a);
        }
        bot operator*(bot::type a) const {
            return bot(this->x*a, this->y*a, this->z*a, this->r*a);
        }
        bot operator/(bot::type a) const {
            return bot(this->x/a, this->y/a, this->z/a, this->r/a);
        }
        friend std::ostream& operator<<(std::ostream& out, const bot& b);
        bot::type x;
        bot::type y;
        bot::type z;
        bot::type r;
        bot::type num_int;
};

std::ostream& operator<<(std::ostream& out, const bot& b) {
    out << "pos=<";
    out << b.x << ",";
    out << b.y << ",";
    out << b.z << ">, r=";
    out << b.r;
    return out;
}

class bot_node {
    public:
        bot_node(const bot& the_bot) {
            this->the_bot = the_bot;
        }
        bot the_bot;
        std::vector<bot_node*> neighbors;
};

/* The BronKerbosch2 algorithm for finding a maximal clique
 * https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
BronKerbosch2(R,P,X):
    if P and X are both empty:
        report R as a maximal clique
    choose a pivot vertex u in P ⋃ X
    for each vertex v in P \ N(u):
        BronKerbosch2(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        P := P \ {v}
        X := X ⋃ {v}
*/

std::vector<bot_node*> BronKerbosch2(std::vector<bot_node*> R, std::vector<bot_node*> P, std::vector<bot_node*> X) {
    if((P.size() == 0)&&(X.size() == 0)) {
        return R;
    }
    std::vector<std::vector<bot_node*>> found_cliques;
    // Choose a pivot vertex with the highest degree.
    size_t max_degree = 0;
    std::vector<bot_node*> pivot_candidates;
    auto manage_candidates = [&pivot_candidates,&max_degree](bot_node* candidate) {
        if(candidate->neighbors.size() > max_degree) {
            max_degree = candidate->neighbors.size();
            pivot_candidates.clear();
            pivot_candidates.push_back(candidate);
        } else if(candidate->neighbors.size() == max_degree) {
            pivot_candidates.push_back(candidate);
        }
    };
    std::for_each(P.begin(), P.end(), manage_candidates);
    std::for_each(X.begin(), X.end(), manage_candidates);
    // Choose the first pivot candidate.
    bot_node* pivot = pivot_candidates[0];

    // build list of vertices to iterate over
    std::vector<bot_node*> vertex_list;
    std::for_each(P.begin(), P.end(), [&vertex_list, &pivot](bot_node* v) {
        if (!hasElement(pivot->neighbors, v)) {
            vertex_list.push_back(v);
        }
    });

    // Perform inner loop.
    for(auto it = vertex_list.begin(); it != vertex_list.end(); ++it) {
        bot_node* v = *it;
        //BronKerbosch2(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        std::vector<bot_node*> new_R = R;
        new_R.push_back(v);

        auto remove_elements = [&pivot](std::vector<bot_node*>& vec) {
            auto it = vec.begin();
            while(it != vec.end()) {
                if(!hasElement(pivot->neighbors,*it)) {
                    it = vec.erase(it);
                } else {
                    ++it;
                }
            }
        };
        std::vector<bot_node*> new_P = P;
        remove_elements(new_P);
        std::vector<bot_node*> new_X = X;
        remove_elements(new_X);
        // Recursive call
        std::vector<bot_node*> max_clique = BronKerbosch2(new_R, new_P, new_X);
        // Save the found clique.
        found_cliques.push_back(max_clique);
        // Move the vertex from P to X.
        for(auto it = P.begin(); it != P.end();) {
            if((*it) == v) {
                P.erase(it);
                break;
            } else {
                ++it;
            }
        }
        X.push_back(v);
    }
    
    // Choose the max clique out of the candidates to return.
    std::vector<bot_node*>& max_clique = found_cliques[0];
    for(auto it = found_cliques.begin()+1; it != found_cliques.end(); ++it) {
        if(it->size() > max_clique.size()) {
            max_clique = *it;
        }
    }
    // Return the max clique.
    return max_clique;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false; ArgParse::ArgParser Parser("Task 46");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

    std::vector<bot_node*> bots;

    bot::type x = 0;
    bot::type y = 0;
    bot::type z = 0;
    bot::type r = 0;
    std::string line;
    while(std::getline(infile, line)) {
        std::istringstream iss(line);
        iss.ignore(5);
        iss >> x;
        iss.ignore(1);
        iss >> y;
        iss.ignore(1);
        iss >> z;
        iss.ignore(5);
        iss >> r;
        bots.push_back(new bot_node(bot(x,y,z,r)));
    }

    if(verbose) {
        std::cout << "There are " << bots.size() << " total bots." << std::endl;
    }

    // We now must find and make connections to the other bots.
    
    for(auto it_1 = bots.begin(); it_1 != bots.end()-1; ++it_1) {
        for(auto it_2 = it_1+1; it_2 != bots.end(); ++it_2) {
            if((*it_1)->the_bot.dist((*it_2)->the_bot) <= (*it_1)->the_bot.r+(*it_2)->the_bot.r) {
                // They intersect!
                (*it_1)->neighbors.push_back(*it_2);
                (*it_2)->neighbors.push_back(*it_1);
            }
        }
    }

    // We must now find the maximal clique!
    
    std::vector<bot_node*> maximal_clique = BronKerbosch2(std::vector<bot_node*>(), bots, std::vector<bot_node*>());

    std::cout << "The maximal clique had size: " << maximal_clique.size() << std::endl;

    if (verbose) {
        std::cout << "The bots were" << std::endl;
        ForEach(maximal_clique, [](const auto& b) {
            std::cout << b->the_bot << std::endl;
        });
    }

    // Now find the furthest bot from the origin. not containing the origin
    bot::type max_min_dist = 0;
    bot origin(0,0,0,0);
    ForEach(maximal_clique, [&max_min_dist,&origin](const auto& bn) {
        if(origin.dist(bn->the_bot) > bn->the_bot.r) {
            // This bot doesn't contain the origin.
            bot::type min_dist = origin.dist(bn->the_bot)-bn->the_bot.r;
            if(min_dist < 0) {
                std::cerr << "We encountered a strange situation!!!" << std::endl;
            }
            if(min_dist > max_min_dist) {
                max_min_dist = min_dist;
            }
        }
    });

    std::cout << "The maximal clique had size: " << maximal_clique.size() << std::endl;
    std::cout << "The smallest distance to the region of maximum signal is: " << max_min_dist << std::endl;

	return 0;
}
