all:
	cd gnu-efi/; make; make bootloader; cd ../; cd kernel; make; make buildimg

run:
	cd kernel/; make run

nfs:
	cd kernel/; make run_no_fs

burndanger:
	dd if=kernel/bin/HSCorpOS.img of=/dev/sdb status=progress
