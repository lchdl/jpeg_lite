#pragma once
#include "basedefs.h"

/* huffman.h: implements Huffman encoding algorithm */
/* Tutorial */
/*
    HuffmanTree<BYTE> tree;                     <= create a Huffman tree with symbol type as "BYTE".
    for (...) {
        tree.recordSymbol(...);                 <= record all symbols occurred in a specified range.
    }
    tree.buildTree();                           <= build Huffman tree.

    Array<BYTE>      all_symbols;               <= all encoded symbols
    Array<Bitstream> all_codes;                 <= Huffman code for all symbols

    tree.enumSymbols(&all_symbols, &all_codes); <= enumerate and save all encoded symbols.
*/

template <typename T>
class HuffmanSymbol {
    /* huffman symbol used in huffman coding */
public:
    HuffmanSymbol() { _frequency = 1; }
    HuffmanSymbol(T _value) { this->_value = _value; _frequency = 1; }
    /* the user may implement their own type of symbol */
    /* (totally not restricted to C++ basic data types), */
    /* as long as they override the operator== to tell the */
    /* compiler how to test if the two symbols are equal. */
    virtual bool operator==(HuffmanSymbol<T>& that) {
        if (this->_value == that._value) return true;
        else return false;
    }
    bool operator!=(HuffmanSymbol<T>& that) {
        return !(this->operator==(that));
    }
    T& operator=(T& that) {
        this->_value = that;
        this->_frequency = 1;
        return this->_value;
    }
    T& operator=(HuffmanSymbol<T>& that) {
        this->_value = that._value;
        this->_frequency = that._frequency;
        return this->_value;
    }
    /* public variables for easy access */
    T _value;
    int _frequency;
};

template <typename T>
class HuffmanNode {
public:
    HuffmanNode* parent;
    HuffmanNode* branches[2]; /* left (0) and right (1) branches */
    HuffmanSymbol<T> hsymbol; /* store Huffman symbol if this node is a leaf node */
    int frequency_sum;        /* sum of frequencies of every symbol under this node */
};

template <typename T>
class HuffmanTree {
public:
    HuffmanTree() { root = NULL; }
    virtual ~HuffmanTree() { deleteTree(); }
    /* record the symbol */
    void recordSymbol(T& symbol) {
        HuffmanSymbol<T> s(symbol);
        for (int i = 0; i < symlib.size(); i++) {
            /* looking for repeated symbol */
            if (symlib[i] == s) { /* may invoke the overloaded operator== */
                /* already in the library */
                /* ok, we add 1 to its frequency */
                symlib[i]._frequency++;
                return;
            }
        }
        /* this is a new symbol */
        s._frequency = 1;
        symlib.append(s);
    }
    /* build Huffman tree */
    void buildTree() {

        symbols.clear();
        codes.clear();
        if (root != NULL) {
            _delete_tree_from(root);
            root = NULL;
        }
        
        Array<HuffmanNode<T>*> active_nodes; /* current active nodes, intermediate variable */

        /* initialize all leaf nodes */
        for (int i = 0; i < symlib.size(); i++) {
            HuffmanNode<T>* n = new HuffmanNode<T>();
            n->branches[0] = NULL;
            n->branches[1] = NULL;
            n->parent = NULL;
            n->hsymbol._value = symlib[i]._value;
            n->hsymbol._frequency = symlib[i]._frequency;
            n->frequency_sum = n->hsymbol._frequency;
            /* put the recently added node to active list */
            active_nodes.append(n);
        }

        while (active_nodes.size() > 1) {

            /* sort nodes based on frequency (ascending order) */
            for (int i = 0; i < active_nodes.size(); i++) {
                for (int j = i + 1; j < active_nodes.size(); j++) {
                    if (active_nodes[i]->frequency_sum > active_nodes[j]->frequency_sum) {
                        HuffmanNode<T>* temp_ptr = active_nodes[i];
                        active_nodes[i] = active_nodes[j];
                        active_nodes[j] = temp_ptr;
                    }
                }
            }

            /* create a new node and link the top two most active nodes */
            HuffmanNode<T>* top1 = active_nodes[0];
            HuffmanNode<T>* top2 = active_nodes[1];
            HuffmanNode<T>* new_node = new HuffmanNode<T>();
            new_node->branches[0] = top1;
            new_node->branches[1] = top2;
            new_node->parent = NULL;
            new_node->frequency_sum = top1->frequency_sum + top2->frequency_sum;
            new_node->hsymbol._frequency = -1;
            top1->parent = new_node;
            top2->parent = new_node;

            /* remove the top two most active nodes and put the newly created
            node into the active node list */
            Array<HuffmanNode<T>*> new_active_nodes;
            for (int i = 2; i < active_nodes.size(); i++) {
                new_active_nodes.append(active_nodes[i]);
            }
            active_nodes = new_active_nodes; /* remove the top two most active nodes */
            active_nodes.append(new_node);

        }

        /* ok, now only one node remaining, set this node as root node and we are finished */
        root = active_nodes[0];

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        /* generate mappings between the symbols and their code  */
        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        Bitstream current_code;
        _map_symbols_from(root, &current_code, &symbols, &codes);
        /* sort symbols */
        for (int i = 0; i < symbols.size(); i++) {
            for (int j = i + 1; j < symbols.size(); j++) {
                if (codes[i] > codes[j]) {
                    T temp_symbol = symbols[i];
                    Bitstream temp_bs = codes[i];
                    symbols[i] = symbols[j];
                    symbols[j] = temp_symbol;
                    codes[i] = codes[j];
                    codes[j] = temp_bs;
                }
            }
        }
        /* remap symbols */
        Bitstream new_code;
        for (int i = 0; i < symbols.size(); i++) {
            Bitstream& current_code = codes[i];
            while (new_code.size() < current_code.size())
                new_code.appendBit(0);
            
            current_code = new_code;
            new_code.inc();
        }


    }
    /* enumerate all symbols */
    void enumSymbols(Array<T>* _symbols, Array<Bitstream>* _codes) {
        for (int i = 0; i < this->symbols.size(); i++) {
            _symbols->append(this->symbols[i]);
            _codes->append(this->codes[i]);
        }
    }
    /* delete Huffman tree */
    void deleteTree() {
        if (root != NULL) {
            _delete_tree_from(root);
            root = NULL;
        }
        symlib.clear();
        symbols.clear();
        codes.clear();
    }
protected:
    //void _print_status() {
    //    /* debug: print symbol library */
    //    printf("- - - - - - - - - - -\n");
    //    printf("symbols:\n");
    //    for (int i = 0; i < hsymbol_library.size(); i++) {
    //        printf("0x%02X : freq=%d\n", hsymbol_library[i]._value, hsymbol_library[i]._frequency);
    //    }
    //    printf("active nodes:\n");
    //    for (int i = 0; i < active_nodes.size(); i++) {
    //        if (active_nodes[i]->branches[0] == NULL || active_nodes[i]->branches[1] == NULL) {
    //            /* this node is a leaf node */
    //            printf("0x%p : sym=0x%02X, freq_sum=%d\n",
    //                active_nodes[i],
    //                active_nodes[i]->hsymbol._value,
    //                active_nodes[i]->frequency_sum);
    //        }
    //        else {
    //            /* not a leaf node */
    //            printf("0x%p : branches(0|1)=(0x%p|0x%p), freq_sum=%d\n",
    //                active_nodes[i],
    //                active_nodes[i]->branches[0], active_nodes[i]->branches[1],
    //                active_nodes[i]->frequency_sum);
    //        }
    //    }
    //}
    void _map_symbols_from(HuffmanNode<T>* node, Bitstream* current_code,
        Array<T>* symbols, Array<Bitstream>* codes) {
        if (node->branches[0] == NULL || node->branches[1] == NULL) {
            /* leaf node */
            symbols->append(node->hsymbol._value);
            codes->append(*current_code);
            return;
        }
        else {
            Bitstream left_code = *current_code;
            Bitstream right_code = *current_code;
            BYTE zero = 0x00, one = 0x01;
            left_code.appendBit(zero);
            right_code.appendBit(one);
            _map_symbols_from(node->branches[0], &left_code, symbols, codes);
            _map_symbols_from(node->branches[1], &right_code, symbols, codes);
        }
    }
    void _delete_tree_from(HuffmanNode<T>* node) {
        if (node->branches[0] == NULL || node->branches[1] == NULL) {
            /* leaf node */
            delete node;
            return;
        }
        else {
            _delete_tree_from(node->branches[0]);
            _delete_tree_from(node->branches[1]);
            delete node;
        }
    }
protected:
    HuffmanNode<T>* root;
    Array<HuffmanSymbol<T>> symlib; /* unique symbol library */

    /* store symbol code mappings */
    Array<T> symbols;
    Array<Bitstream> codes;
};
