//
// Created by d3m3vilurr on 09/05/16.
//

#include "UpdateInfoKor.h"

UpdateInfoKor::UpdateInfoKor(int deviceType) {

    this->region = "KOR";
    this->version = "9.0.0-16";

    if (deviceType == 2 || deviceType == 4) {

        this->model = "n3DS";
        // not support kor n3ds

    } else {

        this->model = "o3DS";

        items.push_back(UpdateItem("b27d85b97fd94fd6fb78bf713cc0dd4f", "/updates/000400DB00010302.cia"));
        items.push_back(UpdateItem("6f221e6c3662cb840f9d86dc9689efc8", "/updates/000400DB00010502.cia"));
        items.push_back(UpdateItem("76a830735986a707e85424eac12d7eb3", "/updates/000400DB00016502.cia"));
        items.push_back(UpdateItem("19b9772543566a257788dc1b6ce7f4f9", "/updates/000400DB00017502.cia"));
        items.push_back(UpdateItem("e8cae7dd5c8c4d5a7d453f1a1e806ab1", "/updates/0004001B00010002.cia"));
        items.push_back(UpdateItem("3bd5e62beebe124ea662f9fc89815dcf", "/updates/0004001B00010702.cia"));
        items.push_back(UpdateItem("37899eaad8fa2e958bb10913934880d8", "/updates/0004001B00010802.cia"));
        items.push_back(UpdateItem("6a0fac95a4efb1580e3a840863cf6474", "/updates/0004009B00011A02.cia"));
        items.push_back(UpdateItem("b36876609c4362ae28211c0c3da46045", "/updates/0004009B00010202.cia"));
        items.push_back(UpdateItem("e78b5fded4191d9669314ad457590c1b", "/updates/0004009B00010402.cia"));
        items.push_back(UpdateItem("6d7673c8487dc231002f1ddc6ee79bf3", "/updates/0004009B00010602.cia"));
        items.push_back(UpdateItem("053be6a5661cbc5076a50fb33df98026", "/updates/0004009B00012502.cia"));
        items.push_back(UpdateItem("e404941e44ad34885084520baefc1531", "/updates/0004009B00013502.cia"));
        items.push_back(UpdateItem("7970c352474532e7b1738e9988b5f09a", "/updates/0004009B00014002.cia"));
        items.push_back(UpdateItem("4eb03910a44a5e25978c7ffaf6a84a03", "/updates/0004009B00014102.cia"));
        items.push_back(UpdateItem("aa170b789a51bd4cf310d815178d53fc", "/updates/0004009B00014202.cia"));
        items.push_back(UpdateItem("10c1345669c8f1fe0ab4b1cad605d6c2", "/updates/0004009B00014302.cia"));
        items.push_back(UpdateItem("e746a051344e49ca625ad76091fa21cf", "/updates/0004009B00015502.cia"));
        items.push_back(UpdateItem("19208ad79ec6c262573448e10a754e5e", "/updates/0004800F484E4C41.cia"));
        items.push_back(UpdateItem("b2b788d315eb4633bc3a0258dd3b1015", "/updates/0004800F484E4841.cia"));
        items.push_back(UpdateItem("f469bb060dcc815dd0f88ffac3e25f46", "/updates/00048005484E444B.cia"));
        items.push_back(UpdateItem("df179e74f764c30efcaa2f50437f56a2", "/updates/000400300000A902.cia"));
        items.push_back(UpdateItem("f0dbc5fa13eeb948ee8fa20b09e16f37", "/updates/000400300000AA02.cia"));
        items.push_back(UpdateItem("02ff4b4c1bf43ff463b247bed54b4487", "/updates/000400300000AC02.cia"));
        items.push_back(UpdateItem("c4589271cabe72ced05879dee91c113c", "/updates/000400300000AD02.cia"));
        items.push_back(UpdateItem("78e5f7143271101f68b8378c8a3e31f6", "/updates/000400300000AE02.cia"));
        items.push_back(UpdateItem("910e6c0d1027c75a82ea24daf1f8c53e", "/updates/000400300000AF02.cia"));
        items.push_back(UpdateItem("700bbbbc22c26c4b5e166526ca242e44", "/updates/000400300000B002.cia"));
        items.push_back(UpdateItem("53c2edea8861f50d49302e94c8c875d3", "/updates/000400300000CF02.cia"));
        items.push_back(UpdateItem("59aac0c3b78d925d9c06a599fd535923", "/updates/000400300000CF03.cia"));
        items.push_back(UpdateItem("06322a3a454b2ab619d5f4d304b528e2", "/updates/000400300000D502.cia"));
        items.push_back(UpdateItem("ed0e86b423edb6523af02c5e83b9e4ec", "/updates/000400300000DE02.cia"));
        items.push_back(UpdateItem("fec585a4d85fe26cd3a014c47500a0f7", "/updates/000400300000DE03.cia"));
        items.push_back(UpdateItem("23cd5cf4b77aff1087d2d863cc93ec14", "/updates/000400300000DF02.cia"));
        items.push_back(UpdateItem("88caac6fbc96afefafd185c599b15f50", "/updates/000400300000E102.cia"));
        items.push_back(UpdateItem("9b902a1c1244b39bb035dc01846c6353", "/updates/000400300000E202.cia"));
        items.push_back(UpdateItem("f0151ec0574ce5103bdc0d395524ecc2", "/updates/000400300000E302.cia"));
        items.push_back(UpdateItem("9a3bff88896144d351887abe75be2121", "/updates/0004001000027A00.cia"));
        items.push_back(UpdateItem("1886507393b2521c82dbcc4c59a84f5c", "/updates/0004001000027D00.cia"));
        items.push_back(UpdateItem("0a8c55d7e6c30956bc4303cd2c23d352", "/updates/0004001000027E00.cia"));
        items.push_back(UpdateItem("52ae58600e8b51a7e8a4ce84c19b5bae", "/updates/0004001000027F00.cia"));
        items.push_back(UpdateItem("c4b629e3b29772b01c027e0271e6e351", "/updates/0004003000008A02.cia"));
        items.push_back(UpdateItem("88a8914656366151f7c72c05d553859a", "/updates/0004013000001A02.cia"));
        items.push_back(UpdateItem("94cd6bb6efb2ce35f279fa5c3af0f4ca", "/updates/0004013000001A03.cia"));
        items.push_back(UpdateItem("878087f71e996fe9084ba1aa3d05caf8", "/updates/0004013000001B02.cia"));
        items.push_back(UpdateItem("9fb5c904cad65a87fb6720bf4730af8b", "/updates/0004013000001B03.cia"));
        items.push_back(UpdateItem("8adc9c421a65ad4f7c29f497bb6c3f42", "/updates/0004013000001C02.cia"));
        items.push_back(UpdateItem("1cd5c8bb84097198ee83b832b14ccc99", "/updates/0004013000001C03.cia"));
        items.push_back(UpdateItem("70fcaa9466840f77198cfc3af26cb1e4", "/updates/0004013000001D02.cia"));
        items.push_back(UpdateItem("6e4dc80cbf34824b584208b57c97d3cc", "/updates/0004013000001D03.cia"));
        items.push_back(UpdateItem("bd38b3217b6e98eeb5e4c2550bce6a8d", "/updates/0004013000001E02.cia"));
        items.push_back(UpdateItem("5b41ffc08dac696aa7bdde15485f9067", "/updates/0004013000001E03.cia"));
        items.push_back(UpdateItem("2af675dc5740bac0407a49b2a665f55d", "/updates/0004013000001F02.cia"));
        items.push_back(UpdateItem("9c98936d8ba02db18fea0a4cfe203220", "/updates/0004013000001F03.cia"));
        items.push_back(UpdateItem("1a8c61842c88471640265149544018d8", "/updates/0004013000002A02.cia"));
        items.push_back(UpdateItem("65b0a9ae7d7b27bdeebb6433b6b838f4", "/updates/0004013000002A03.cia"));
        items.push_back(UpdateItem("c708ee846a01e78ce33f1df1dc063b93", "/updates/0004013000002B02.cia"));
        items.push_back(UpdateItem("6938c0fed001682a4c1a0a84c7e89b23", "/updates/0004013000002C02.cia"));
        items.push_back(UpdateItem("bbaf00da95bcd48ba30db9c29bc5d710", "/updates/0004013000002D02.cia"));
        items.push_back(UpdateItem("7bc4adecfccb8cc5665daef4dec20836", "/updates/0004013000002D03.cia"));
        items.push_back(UpdateItem("53e56a11ad82534142f4c2aff65b8ce0", "/updates/0004013000002E02.cia"));
        items.push_back(UpdateItem("14f24e1ba6f840680ca4fa7d9b69b6f1", "/updates/0004013000002E03.cia"));
        items.push_back(UpdateItem("73130495744f6881e0f9a68897a1886a", "/updates/0004013000002F02.cia"));
        items.push_back(UpdateItem("14d067ecb8fa9180e59ee839a1fb83b0", "/updates/0004013000002F03.cia"));
        items.push_back(UpdateItem("0c77fdd2e5fd4225f831fca96a23923d", "/updates/0004001000027000.cia"));
        items.push_back(UpdateItem("d3beaffdd52080e8d9696acf5a7cdadd", "/updates/0004001000027100.cia"));
        items.push_back(UpdateItem("b000095e43ab92cbb3a72cbbbdf71ef7", "/updates/0004001000027200.cia"));
        items.push_back(UpdateItem("48469c328bfda43c990158836ebec959", "/updates/0004001000027300.cia"));
        items.push_back(UpdateItem("f4db44113cb1e4ec19c3b59125f93649", "/updates/0004001000027400.cia"));
        items.push_back(UpdateItem("8c34994d030274265952a18106290338", "/updates/0004001000027500.cia"));
        items.push_back(UpdateItem("549501f35a9c67ac01f42135ea4b37e6", "/updates/0004001000027700.cia"));
        items.push_back(UpdateItem("7a90b9acbd99ece4d3c013320270927a", "/updates/0004001000027800.cia"));
        items.push_back(UpdateItem("9d3329dd0466fa4f9eac1db940358707", "/updates/0004001000027900.cia"));
        items.push_back(UpdateItem("d54e05f09eefdb2acdd9aad5b5f3fda9", "/updates/0004013000001502.cia"));
        items.push_back(UpdateItem("69d36484ad3b32ea6179cbe00423b937", "/updates/0004013000001503.cia"));
        items.push_back(UpdateItem("fd8fe9fbf0594870f7049c4ba74260f3", "/updates/0004013000001602.cia"));
        items.push_back(UpdateItem("d8eea6c7f5a906b4d8226eff5ffa1a22", "/updates/0004013000001702.cia"));
        items.push_back(UpdateItem("d091987acd5bfeb60778e1a1f36ac07d", "/updates/0004013000001703.cia"));
        items.push_back(UpdateItem("0545e69671b600f7729a6cb3bf9f663d", "/updates/0004013000001802.cia"));
        items.push_back(UpdateItem("7d9b40bdfdb0e21cb920df026c560c3f", "/updates/0004013000001803.cia"));
        items.push_back(UpdateItem("e8e9886d6e301548a8e292ec39685218", "/updates/0004013000002002.cia"));
        items.push_back(UpdateItem("6fcf72e818cfeb4057b253898acc3b57", "/updates/0004013000002102.cia"));
        items.push_back(UpdateItem("22db18022cd83f0696bb8ec594eb9958", "/updates/0004013000002103.cia"));
        items.push_back(UpdateItem("173512e394072037613a1f5788de2afd", "/updates/0004013000002202.cia"));
        items.push_back(UpdateItem("370090ef93b810585f20d6fb57ec6d9d", "/updates/0004013000002203.cia"));
        items.push_back(UpdateItem("65479e968105ea60ab01ac63cb51cd87", "/updates/0004013000002302.cia"));
        items.push_back(UpdateItem("88e8d05c03c8abe7f90488ffcdc4542c", "/updates/0004013000002303.cia"));
        items.push_back(UpdateItem("d709c39abd711bc96d00b4ffc236adfb", "/updates/0004013000002402.cia"));
        items.push_back(UpdateItem("51fdb8536eb83762b4dd57800cde08a0", "/updates/0004013000002403.cia"));
        items.push_back(UpdateItem("7cf379b8438556cf756b50519090f071", "/updates/0004013000002602.cia"));
        items.push_back(UpdateItem("c242661cb22b900bb8095c0f86a628aa", "/updates/0004013000002702.cia"));
        items.push_back(UpdateItem("f4992a20462a4a41aef37d897aea9b08", "/updates/0004013000002703.cia"));
        items.push_back(UpdateItem("c7ca7097ecb2326d7b877ed90777d5c6", "/updates/0004013000002802.cia"));
        items.push_back(UpdateItem("4cd549922730653f80cc673e07f80e84", "/updates/0004013000002902.cia"));
        items.push_back(UpdateItem("fca89ddc17e3e892516b0f81b8958743", "/updates/0004013000002903.cia"));
        items.push_back(UpdateItem("fad6e0d994045889b406b8564111c26d", "/updates/0004013000003102.cia"));
        items.push_back(UpdateItem("d85788aefb07b6c07b1da9b84ee017b1", "/updates/0004013000003103.cia"));
        items.push_back(UpdateItem("903fcca30e252a558339ba9be36e64b8", "/updates/0004013000003202.cia"));
        items.push_back(UpdateItem("69e1981acea2949da61ec2612ef5a6c8", "/updates/0004013000003203.cia"));
        items.push_back(UpdateItem("2bde1d8af1d1cdc247417f6d289b2454", "/updates/0004013000003302.cia"));
        items.push_back(UpdateItem("59050620268e9409d8678ab04b366288", "/updates/0004013000003303.cia"));
        items.push_back(UpdateItem("f40fd755269c4150b84978b41d5abf7b", "/updates/0004013000003402.cia"));
        items.push_back(UpdateItem("663fef2ef61eb61fcff2ab898885802a", "/updates/0004013000003502.cia"));
        items.push_back(UpdateItem("70d9f078c0728c72b2311e6a2a234f68", "/updates/0004013000003702.cia"));
        items.push_back(UpdateItem("05386ec81b9a37f316156d5b43e2b3e5", "/updates/0004013000003802.cia"));
        items.push_back(UpdateItem("b72acac949ecee3a0af47b4ab0a6f87a", "/updates/0004013000008002.cia"));
        items.push_back(UpdateItem("899da982f618800143c2bffe8a52998c", "/updates/0004013800000002.cia"));
        items.push_back(UpdateItem("4d284baac2a26b504a7380b53bca02e0", "/updates/0004013800000003.cia"));
        items.push_back(UpdateItem("f36757b1070f0c7d2321915a82b5da92", "/updates/0004013800000102.cia"));
        items.push_back(UpdateItem("e32824752ab08d57532fed8d37dbf242", "/updates/0004013800000202.cia"));
        items.push_back(UpdateItem("40ea1a5aab4c6260a8513606bbec67f0", "/updates/0004800542383841.cia"));
    }

}
