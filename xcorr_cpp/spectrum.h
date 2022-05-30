#ifndef SPECTRUM_H
#define SPECTRUM_H

class spectrum{
    private:
        float mass;
        int charge;
        unsigned start;
        unsigned end;
        unsigned length;
        unsigned id {0};
    public:
        /*Target constructor */
        spectrum(float m, int c, unsigned s, unsigned e, unsigned l, unsigned i):
            mass(m), charge(c), start(s), end(e), length(l), id(i){}
        
        spectrum(): spectrum(0, 0, 0, 0 ,0, 0) {}

        float get_mass() {return mass;}
        int get_charge() {return charge;}
        unsigned get_start() {return start;}
        unsigned get_id() {return id;}
        unsigned get_end() {return end;}
        unsigned get_length() {return length;}

        void set_mass(float m) {mass = m;}
        void set_charge(int c) {charge = c;}
        void set_start(unsigned s) {start = s;}
        void set_end(unsigned e) {end = e;}
        void set_length(unsigned l) {length = l;}
        void set_id(unsigned i) {id = i;}
};


#endif