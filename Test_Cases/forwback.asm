	.text
begin:
	lw	$1,0($28)
	lw	$2,4($28)
	ori	$3,$1,20
	bne	$3,$0,label1
	j	label2
label1:
	add	$10,$1,$1
label2:
	beq	$3,$1,begin
	add	$15,$1,$2
error:
	j	label1