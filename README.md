# Bem vindo ao Lane Rush

Este é um jogo de carro muito divertido e leve que pode rodar facilmente em seu PC, basta seguir as instruções.

# Instruções para Linux

1- Acessar o VS Code

2- instalar as bibliotecas caso necessário

3- Rodar os seguintes comandos:

 conan profile detect --force
 
 export CMAKE_POLICY_VERSION_MINIMUM=3.5 
 
 conan install . --output-folder=build --build=missing
 
 cd build
 
 cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
 
 cmake --build .

 ./JogoNave

# Instruções para o Windows

1- Acessar o VS Code

2- instalar as bibliotecas caso necessário

3- Rodar os seguintes comandos:

 conan profile detect --force

 $env:CMAKE_POLICY_VERSION_MINIMUM="3.5"

 conan install . --output-folder=build --build=missing

 cd build

 cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

 cmake --build . --config Release

 ./JogoNave.exe
 
# Caso tenha problemas para rodar

1 - Verifique em qual Sistema Operacional você está

2-  Verifique se as bibliotecas estão todas instaladas

3-  Verifique se as pastas estão na ordem correta

4- Caso nenhum desses resolva, mande a mensagem de erro para uma Inteligência Artificial para identificar o problema.
