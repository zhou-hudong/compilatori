
void Loop1 (){
  int a = 0;
  int b = 0;
  
  if (a == 0){
    for(int i=0; i<5; i++){
      a = a + 1;
    }
  }
  
  for(int i=0; i<5; i++){
    b = b + 1;
  }
}


void Loop2 (){
  int a = 0;
  int b = 0;
  
  for(int i=0; i<5; i++){
    a = a + 1;
  }
  
  for(int i=0; i<5; i++){
    b = b + 1;
  }
}


void Loop3 (){
  int a = 0;
  int b = 0;
  
  for(int i=0; i<5; i++){
    a = a + 1;
  }
  
  int c = a * 2;
  
  for(int i=0; i<5; i++){
    b = b + 1;
  }
}


void Loop4 (){
  int a = 0;
  int b = 0;
  
  for(int i=0; i<5; i++){
    a = a + 1;
  }
  
  if(a>0){
    int c = a * 2;
  }
  
  for(int i=0; i<5; i++){
    b = b + 1;
  }
}


void Loop5 (){
  int a = 0;
  int b = 0;
  int n = 5;
  
  if (a == 0){
    for(int i=0; i<n; i++){
      a = a + 1;
    }
    for(int i=0; i<5; i++){
      b = b + 1;
    }
  }
}


void Loop6 (){
  int a = 0;
  int b = 0;
  
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      a = a + 1;
    }
  }
  
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      b = b + 1;
    }
  }

}


void Loop7 (){
  int a = 0;
  int b = 0;
  int c = 0;
  
  if (c == 0){
    for(int i=0; i<5; i++){
      a = a + 1;
    }
  }
  
  if (c == 0){
    for(int i=0; i<5; i++){
      b = b + 1;
    }
  }
}


void Loop8 (){
  int a[5];
  int b[5];
  
  for(int i=0; i<5; i++){
    a[i] = i + 1;
  }
  
  for(int i=0; i<5; i++){
    b[i] = a[i + 3] + 1;
  }
}
