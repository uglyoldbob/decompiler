//A simple source file
void function_1(void) {
	//Dummy code 0
	//Dummy code 1
}

void function_2(void) {
	do {
		//Dummy code 0
	} while (?);
	//Dummy code 1
}

void function_3(void) {
	do {
		//Dummy code 0
		//Dummy code 1
	} while (1);
}

void function_4(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 1
	} while (1);
}

void function_5(void) {
	//Dummy code 0
	do {
		//Dummy code 1
	} while (1);
}

void function_6(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 1
	} while (1);
}

void function_7(void) {
	if (?) {
		//Dummy code 2
	}
	else {
		//Dummy code 1
	}
}

void function_8(void) {
	//Dummy code 0
	while (?) {
		//Dummy code 1
	}
	//Dummy code 2
}

void function_9(void) {
	if (?) {
		//Dummy code 2
	}
	else {
		do {
			//Dummy code 1
		} while (1);
	}
}

void function_10(void) {
	//Dummy code 0
	//Dummy code 1
	//Dummy code 2
}

void function_11(void) {
	do {
		//Dummy code 0
	} while (?);
	//Dummy code 1
	//Dummy code 2
}

void function_12(void) {
	//Dummy code 0
	if (?) {
		//Dummy code 1
	}
	//Dummy code 2
}

void function_13(void) {
	//Dummy code 0
	while (?) {
		do {
			//Dummy code 1
		} while (?);
	}
	//Dummy code 2
}

void function_14(void) {
	do {
		//Dummy code 0
		//Dummy code 1
	} while (?);
	//Dummy code 2
}

void function_15(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 1
	} while (?);
	//Dummy code 2
}

void function_16(void) {
	#error not simplified
	//Dummy code 2
	//Dummy code 0
	//Dummy code 1
}

void function_17(void) {
	//Dummy code 0
	do {
		//Dummy code 1
	} while (?);
	//Dummy code 2
}

void function_18(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 1
	} while (?);
	//Dummy code 2
}

void function_19(void) {
	//Dummy code 0
	if (?) {
		do {
			//Dummy code 1
		} while (?);
	}
	//Dummy code 2
}

void function_20(void) {
	//Dummy code 0
	while (?) {
		//Dummy code 2
	}
	//Dummy code 1
}

// does not look correct
void function_21(void) {
	do {
		//Dummy code 0
		while (?) {
			//Dummy code 2
		}
		//Dummy code 1
	} while (1);
}

void function_22(void) {
	//Dummy code 0
	while (?) {
		//Dummy code 2
	}
	do {
		//Dummy code 1
	} while (1);
}

void function_23(void) {
	do {
		//Dummy code 0
		//Dummy code 1
		//Dummy code 2
	} while (1);
}

void function_24(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 1
		//Dummy code 2
	} while (1);
}

void function_25(void) {
	do {
		//Dummy code 0
		if (?) {
			//Dummy code 1
		}
		//Dummy code 2
	} while (1);
}

//looks wrong
void function_26(void) {
	do {
		//Dummy code 0
		while (?) {
			//Dummy code 2
		}
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_27(void) {
	do {
		do {
			//Dummy code 0
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_28(void) {
	do {
		do {
			do {
				//Dummy code 0
			} while (?);
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_29(void) {
	#error not simplified
	//Dummy code 2
	//Dummy code 0
	//Dummy code 1
}

void function_30(void) {
	do {
		//Dummy code 0
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_31(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_32(void) {
	do {
		//Dummy code 0
		if (?) {
			do {
				//Dummy code 1
			} while (?);
		}
		//Dummy code 2
	} while (1);
}

void function_33(void) {
	//Dummy code 0
	//Dummy code 2
	//Dummy code 1
}

void function_34(void) {
	do {
		//Dummy code 0
	} while (?);
	//Dummy code 2
	//Dummy code 1
}

void function_35(void) {
	//Dummy code 0
	if (?) {
		//Dummy code 2
	}
	//Dummy code 1
}

void function_36(void) {
	do {
		//Dummy code 0
		//Dummy code 2
		//Dummy code 1
	} while (1);
}

void function_37(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 2
		//Dummy code 1
	} while (1);
}

void function_38(void) {
	do {
		//Dummy code 0
		if (?) {
			//Dummy code 2
		}
		//Dummy code 1
	} while (1);
}

void function_39(void) {
	//Dummy code 0
	//Dummy code 2
	do {
		//Dummy code 1
	} while (1);
}

void function_40(void) {
	do {
		//Dummy code 0
	} while (?);
	//Dummy code 2
	do {
		//Dummy code 1
	} while (1);
}

void function_41(void) {
	//Dummy code 0
	if (?) {
		//Dummy code 2
	}
	do {
		//Dummy code 1
	} while (1);
}

void function_42(void) {
	//Dummy code 0
	do {
		//Dummy code 1
		//Dummy code 2
	} while (1);
}

void function_43(void) {
	//Dummy code 0
	do {
		//Dummy code 2
		//Dummy code 1
	} while (1);
}

void function_44(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 1
		//Dummy code 2
	} while (1);
}

void function_45(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 2
		//Dummy code 1
	} while (1);
}

void function_46(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 1
	//Dummy code 2
}

void function_47(void) {
	do {
		//Dummy code 0
		//Dummy code 2
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_48(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 2
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_49(void) {
	do {
		//Dummy code 0
		if (?) {
			//Dummy code 2
		}
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

//looks wrong
void function_50(void) {
	do {
		//Dummy code 0
		//Dummy code 1
		while (?) {
			//Dummy code 2
		}
	} while (1);
}

void function_51(void) {
	do {
		//Dummy code 0
		do {
			//Dummy code 2
			//Dummy code 1
		} while (?);
	} while (1);
}

//looks wrong
void function_52(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 1
		while (?) {
			//Dummy code 2
		}
	} while (1);
}

void function_53(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		do {
			//Dummy code 2
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_54(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 1
	//Dummy code 2
}

void function_55(void) {
	//Dummy code 0
	do {
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_56(void) {
	//Dummy code 0
	do {
		//Dummy code 2
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_57(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 2
	} while (1);
}

void function_58(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 2
		do {
			//Dummy code 1
		} while (?);
	} while (1);
}

void function_59(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 2
	do {
		//Dummy code 1
	} while (?);
}

void function_60(void) {
	if (?) {
		do {
			//Dummy code 2
		} while (1);
	}
	else {
		//Dummy code 1
	}
}

void function_61(void) {
	//Dummy code 0
	while (?) {
		//Dummy code 1
	}
	do {
		//Dummy code 2
	} while (1);
}

void function_62(void) {
	if (?) {
		do {
			//Dummy code 2
		} while (1);
	}
	else {
		do {
			//Dummy code 1
		} while (1);
	}
}

void function_63(void) {
	//Dummy code 0
	//Dummy code 1
	do {
		//Dummy code 2
	} while (1);
}

void function_64(void) {
	do {
		//Dummy code 0
	} while (?);
	//Dummy code 1
	do {
		//Dummy code 2
	} while (1);
}

void function_65(void) {
	//Dummy code 0
	if (?) {
		//Dummy code 1
	}
	do {
		//Dummy code 2
	} while (1);
}

void function_66(void) {
	//Dummy code 0
	while (?) {
		do {
			//Dummy code 1
		} while (?);
	}
	do {
		//Dummy code 2
	} while (1);
}

void function_67(void) {
	do {
		//Dummy code 0
		//Dummy code 1
	} while (?);
	do {
		//Dummy code 2
	} while (1);
}

void function_68(void) {
	do {
		do {
			//Dummy code 0
		} while (?);
		//Dummy code 1
	} while (?);
	do {
		//Dummy code 2
	} while (1);
}

void function_69(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 1
	do {
		//Dummy code 2
	} while (1);
}

void function_70(void) {
	//Dummy code 0
	do {
		//Dummy code 1
	} while (?);
	do {
		//Dummy code 2
	} while (1);
}

void function_71(void) {
	do {
		//Dummy code 0
	} while (?);
	do {
		//Dummy code 1
	} while (?);
	do {
		//Dummy code 2
	} while (1);
}

void function_72(void) {
	//Dummy code 0
	if (?) {
		do {
			//Dummy code 1
		} while (?);
	}
	do {
		//Dummy code 2
	} while (1);
}

void function_73(void) {
	if (?) {
		//Dummy code 3
	}
	else {
		//Dummy code 1
		//Dummy code 2
	}
}

void function_74(void) {
	if (?) {
		//Dummy code 2
	}
	else {
		//Dummy code 1
		//Dummy code 3
	}
}

void function_75(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 1
	//Dummy code 2
	//Dummy code 3
}

void function_76(void) {
	if (?) {
		//Dummy code 3
	}
	else {
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 2
	}
}

void function_77(void) {
	#error not simplified
	//Dummy code 2
	//Dummy code 3
	//Dummy code 1
	//Dummy code 0
}

void function_78(void) {
	if (?) {
		//Dummy code 2
	}
	else {
		do {
			//Dummy code 1
		} while (?);
		//Dummy code 3
	}
}

//looks wrong
void function_79(void) {
	if (?) {
		//Dummy code 3
	}
	else {
		//Dummy code 2
	}
}

//looks wrong
void function_80(void) {
	if (?) {
		//Dummy code 3
	}
	else {
		//Dummy code 2
	}
}

void function_81(void) {
	#error not simplified
	//Dummy code 0
	//Dummy code 3
	//Dummy code 1
	//Dummy code 2
}

void function_82(void) {
	#error not simplified
	//Dummy code 3
	//Dummy code 1
	//Dummy code 0
	//Dummy code 2
}

