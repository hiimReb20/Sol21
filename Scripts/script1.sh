mkdir ./dir_l
mkdir ./dir_s
mkdir ./file
mkdir ./file_o
echo progetto > /home/reb/rebecca_navari_CorsoA/file/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file/solsol.txt
done

echo progetto > /home/reb/rebecca_navari_CorsoA/file_o/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file_o/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file_o/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file_o/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file/solsol.txt
done
./client.o -h
./client.o -f ./mysock.sk -w /home/reb/rebecca_navari_CorsoA/file_o,5 -a /home/reb/rebecca_navari_CorsoA/file/sol.txt,bellastringa -o 1,/home/reb/rebecca_navari_CorsoA/file/sol.txt,/home/reb/rebecca_navari_CorsoA/file/project.txt -D /home/reb/rebecca_navari_CorsoA/dir_s -r /home/reb/rebecca_navari_CorsoA/file_o/project.txt,/home/reb/rebecca_navari_CorsoA/file_o/project.txt -R 17 -d /home/reb/rebecca_navari_CorsoA/dir_l -t 200 -p -r /home/reb/rebecca_navari_CorsoA/file/sol.txt
