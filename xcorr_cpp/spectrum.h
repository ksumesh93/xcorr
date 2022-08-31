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
        char *data;
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
        float get_mz(unsigned int index){
            int offset = this->start % 4096;
            float *mz = (float*)(this->data + (index*8) + offset);
            return *mz;
        }
        float get_intensity(unsigned int index){
            int offset = this->start % 4096;
            float *inten = (float*)(this->data + (index*8) + offset + 4);
            return *inten;
        }

        void set_data(char *data){ this->data = data;}
        void set_mass(float m) {mass = m;}
        void set_charge(int c) {charge = c;}
        void set_start(unsigned s) {start = s;}
        void set_end(unsigned e) {end = e;}
        void set_length(unsigned l) {length = l;}
        void set_id(unsigned i) {id = i;}
};


#endif