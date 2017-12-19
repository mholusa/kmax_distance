#ifndef _IMAGE_H_
#define _IMAGE_H_

template <class T>
class Image {
  public:
    int width; 
    int height; 
    int size; 
    T* vals;
    
    Image( int w, int h, T* data ) { 
        width = w; height = h; size = w*h; 
        vals = new T[size];
        for(int i = 0; i < size; i++) vals[i] = data[i];
    }
    
    Image( int w, int h ) { 
        width = w; height = h; size = w*h; 
        vals = new T[size];
    }
    
    ~Image() 
    {
        delete[] vals;
    }
    
};

#endif
