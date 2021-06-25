mkdir ./dir_l1
mkdir ./dir_s1
mkdir ./file1
mkdir ./file_o1

mkdir ./dir_l2
mkdir ./dir_s2
mkdir ./file2
mkdir ./file_o2

mkdir ./dir_l3
mkdir ./dir_s3
mkdir ./file3
mkdir ./file_o3

echo progetto > /home/reb/rebecca_navari_CorsoA/file1/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file1/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file1/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file1/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file1/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file1/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file1/solsol.txt
done

echo progetto > /home/reb/rebecca_navari_CorsoA/file_o1/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o1/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file_o1/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file_o1/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file_o1/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o1/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file_o1/solsol.txt
done






echo progetto > /home/reb/rebecca_navari_CorsoA/file2/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file2/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file2/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file2/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file2/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file2/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file2/solsol.txt
done

echo progetto > /home/reb/rebecca_navari_CorsoA/file_o2/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o2/sol.txt
echo project > /home/rebecca_navari_CorsoA/file_o2/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file_o2/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file_o2/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o2/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file_o2/solsol.txt
done






echo progetto > /home/reb/rebecca_navari_CorsoA/file3/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file3/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file3/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file3/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file3/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file3/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file3/solsol.txt
done

echo progetto > /home/reb/rebecca_navari_CorsoA/file_o3/progetto.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o3/sol.txt
echo project > /home/reb/rebecca_navari_CorsoA/file_o3/project.txt
echo pr >> /home/reb/rebecca_navari_CorsoA/file_o3/project.txt
echo in bocca al lupo >> /home/reb/rebecca_navari_CorsoA/file_o3/project.txt
echo sol > /home/reb/rebecca_navari_CorsoA/file_o3/sol.txt
for i in {0..100}
do
    echo "$i" >> /home/reb/rebecca_navari_CorsoA/file_o3/solsol.txt
done

./client.o -f ./mysock.sk -w /home/reb/rebecca_navari_CorsoA/file_o1,5 -a /home/reb/rebecca_navari_CorsoA/file1/sol.txt,bellastringa -o 1,/home/reb/rebecca_navari_CorsoA/file1/sol.txt,/home/reb/rebecca_navari_CorsoA/file1/project.txt -D /home/reb/rebecca_navari_CorsoA/dir_s1 -r /home/reb/rebecca_navari_CorsoA/file_o1/project.txt,/home/reb/rebecca_navari_CorsoA/file_o1/project.txt -R 17 -d /home/reb/rebecca_navari_CorsoA/dir_l1 -t 200 -p -r /home/reb/rebecca_navari_CorsoA/file1/sol.txt
./client.o -f ./mysock.sk -w /home/reb/rebecca_navari_CorsoA/file_o2,5 -a /home/reb/rebecca_navari_CorsoA/file2/sol.txt,bellastringa -o 1,/home/reb/rebecca_navari_CorsoA/file2/sol.txt,/home/reb/rebecca_navari_CorsoA/file2/project.txt -D /home/reb/rebecca_navari_CorsoA/dir_s2-r /home/reb/rebecca_navari_CorsoA/file_o2/project.txt,/home/reb/rebecca_navari_CorsoA/file_o2/project.txt -R 17 -d /home/reb/rebecca_navari_CorsoA/dir_l2 -t 200 -p -r /home/reb/rebecca_navari_CorsoA/file2/sol.txt
./client.o -f ./mysock.sk -w /home/reb/rebecca_navari_CorsoA/file_o3,5 -a /home/reb/rebecca_navari_CorsoA/file3/sol.txt,bellastringa -o 1,/home/reb/rebecca_navari_CorsoA/file3/sol.txt,/home/reb/rebecca_navari_CorsoA/file3/project.txt -D /home/reb/rebecca_navari_CorsoA/dir_s3 -r /home/reb/rebecca_navari_CorsoA/file_o3/project.txt,/home/reb/rebecca_navari_CorsoA/file_o3/project.txt -R 17 -d /home/reb/rebecca_navari_CorsoA/dir_l3 -t 200 -p -r /home/reb/rebecca_navari_CorsoA/file3/sol.txt
 
